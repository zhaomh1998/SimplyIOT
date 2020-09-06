import time
import logging
import traceback
from threading import Thread
from queue import Queue
from flask import Flask, make_response, request
import VeSync

logger = logging.getLogger(__name__)


def third_party_switch(q):
    # type: (Queue) -> None
    jobs = dict()
    while True:
        # Update jobs
        if not q.empty():
            logger.info('Updating job')
            method, args = q.get()
            assert isinstance(method, str)
            assert method in ['VeSync'], 'Unsupported method %s!' % method
            assert isinstance(args, dict)
            try:
                if method == 'VeSync':
                    device = (args['Aid'], args['Tk'], args['Cid'])
                    config = {'on': int(args['On']), 'off': int(args['Off']), 'state': False, 'last_at': 0,
                              'on_cb': VeSync.VeSyncBare.on, 'off_cb': VeSync.VeSyncBare.off,
                              'active': True}
                    if float(args['Last']) > 0:
                        logger.info('Scheduled to turn off in %.1f seconds' % (float(args['Last']) * 60))
                        config['scheds'] = [
                            {
                                'ts': time.time() + float(args['Last']) * 60,
                                'cb': VeSync.VeSyncBare.off,
                                'active': True
                            }]
                    jobs[device] = config
            except:
                traceback.print_exc()

        for device, config in jobs.items():
            current = time.time()
            if 'scheds' in config.keys():
                for sched in config['scheds']:
                    if sched['active']:
                        if current > sched['ts']:
                            if sched['cb'](*device):
                                sched['active'] = False
                                config['active'] = False
                                continue
                            else:
                                logging.error(f'Failed to turn off {device}. Retrying in 1s')
                                time.sleep(1)
            if config['active']:
                if config['state'] is False:  # It's off currently
                    if config['on'] <= 0:  # Never on
                        if config['off_cb'](*device):
                            config['state'] = False
                            config['active'] = False
                            continue
                        else:
                            logging.error(f'Failed to turn off {device}. Retrying in 1s')
                            time.sleep(1)
                        continue
                    elif current - config['last_at'] < config['off']:  # Timer not expired
                        continue
                    else:
                        if config['on_cb'](*device):
                            config['state'] = True
                            config['last_at'] = time.time()
                        else:
                            logging.error(f'Failed to turn on {device}. Retrying in 1s')
                            time.sleep(1)

                else:  # It's on currently
                    if config['off'] <= 0:  # Never off
                        if config['on_cb'](*device):
                            config['state'] = True
                            config['active'] = False
                            continue
                        else:
                            logging.error(f'Failed to turn on {device}. Retrying in 1s')
                            time.sleep(1)
                    elif current - config['last_at'] < config['on']:  # Timer not expired
                        continue
                    else:
                        if config['off_cb'](*device):
                            config['state'] = False
                            config['last_at'] = time.time()
                        else:
                            logging.error(f'Failed to turn off {device}. Retrying in 1s')
                            time.sleep(1)

        time.sleep(0.1)


class SIOTServer:
    def __init__(self):
        self.scrape_progress = 0
        self.update_queue = Queue(maxsize=10)
        self.last_finished = 0
        self.scrape_thread = Thread(target=third_party_switch, args=(self.update_queue,))

        self.app = Flask(__name__)
        self.scrape_thread.start()
        self.app.add_url_rule(rule='/', view_func=self.default_page)
        self.app.add_url_rule(rule='/api/set_vesync', view_func=self.set_vesync, methods=['PUT'])
        self.app.run(host='0.0.0.0', port=8000, debug=True)

    def default_page(self):
        return make_response('Hello World!')

    def set_vesync(self):
        if all([i in request.headers for i in ['Aid', 'Tk', 'Cid', 'On', 'Off', 'Last']]):
            self.update_queue.put(('VeSync', dict(request.headers)))
            return make_response({'ok': True}), 200
        else:
            return make_response({'ok': False, 'message': 'API syntax error'}), 400


if __name__ == '__main__':
    SIOTServer()
