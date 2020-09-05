import os
import logging
import requests
from pyvesync import VeSync

logger = logging.getLogger(__name__)


class VeSyncWrapper:
    def __init__(self, username, password):
        self.manager = VeSync(username, password)
        self.manager.login()
        self.manager.update()
        self.devices = self.manager.outlets

    def on(self, **kwargs):
        # TODO: Add support for multiple switches
        self.devices[0].turn_on()

    def off(self, **kwargs):
        # TODO: Add support for multiple switches
        self.devices[0].turn_off()

    def ls(self, **kwargs):
        raise NotImplementedError


class VeSyncBare:
    API_BASE_URL = 'https://smartapi.vesync.com'
    API_TIMEOUT = 5

    DEFAULT_TZ = 'America/New_York'

    APP_VERSION = '2.5.1'
    PHONE_BRAND = 'SM N9005'
    PHONE_OS = 'Android'
    MOBILE_ID = '1234567890123456'
    USER_TYPE = '1'

    @staticmethod
    def req_headers(account_id, token):
        headers = {
            'accept-language': 'en',
            'accountId': account_id,
            'appVersion': VeSyncBare.APP_VERSION,
            'content-type': 'application/json',
            'tk': token,
            'tz': VeSyncBare.DEFAULT_TZ
        }
        return headers

    @staticmethod
    def call_api(api: str, method: str,
                 json: dict = None, headers: dict = None):
        #
        response = None
        status_code = None

        try:
            logger.debug("[%s] calling '%s' api" % (method, api))
            if method == 'get':
                r = requests.get(
                    VeSyncBare.API_BASE_URL + api, json=json,
                    headers=headers, timeout=VeSyncBare.API_TIMEOUT
                )
            elif method == 'post':
                r = requests.post(
                    VeSyncBare.API_BASE_URL + api, json=json,
                    headers=headers, timeout=VeSyncBare.API_TIMEOUT
                )
            elif method == 'put':
                r = requests.put(
                    VeSyncBare.API_BASE_URL + api, json=json,
                    headers=headers, timeout=VeSyncBare.API_TIMEOUT
                )
            else:
                raise Exception('Unexpected method %s' % method)
        except requests.exceptions.RequestException as e:
            logger.error(e)
        except Exception as e:
            logger.error(e)
        else:
            if r.status_code == 200:
                status_code = 200
                response = r.json()
            else:
                logger.debug('Unable to fetch %s%s' % (VeSyncBare.API_TIMEOUT, api))
        finally:
            return response, status_code

    @staticmethod
    def on(account_id, token, cid):
        logger.info('Turning on')
        _, status_code = VeSyncBare.call_api(
            '/v1/wifi-switch-1.3/' + cid + '/status/on',
            'put',
            headers=VeSyncBare.req_headers(account_id, token)
        )
        if status_code is not None and status_code == 200:
            return True
        else:
            logger.warning('Error turning on: status_code=%d'.format(status_code))
            return False

    @staticmethod
    def off(account_id, token, cid):
        logger.info('Turning off')
        _, status_code = VeSyncBare.call_api(
            '/v1/wifi-switch-1.3/' + cid + '/status/off',
            'put',
            headers=VeSyncBare.req_headers(account_id, token)
        )
        if status_code is not None and status_code == 200:
            return True
        else:
            logger.warning('Error turning on/off: status_code=%d'.format(status_code))
            return False


if __name__ == '__main__':
    VS_USERNAME = os.getenv('VESYNC_TEST_USER')
    VS_PASS = os.getenv('VESYNC_TEST_PASS')
    VS_AID = os.getenv('VESYNC_TEST_AID')
    VS_TK = os.getenv('VESYNC_TEST_TK')
    VS_CID = os.getenv('VESYNC_TEST_CID')

    assert VS_USERNAME is not None, 'Please set environment variable \'VESYNC_TEST_USER\''
    assert VS_PASS is not None, 'Please set environment variable \'VESYNC_TEST_PASS\''
    assert VS_AID is not None, 'Please set environment variable \'VESYNC_TEST_AID\''
    assert VS_TK is not None, 'Please set environment variable \'VESYNC_TEST_TK\''
    assert VS_CID is not None, 'Please set environment variable \'VESYNC_TEST_CID\''
    manager = VeSyncWrapper(VS_USERNAME, VS_PASS)
    manager.on()
    VeSyncBare.off(VS_AID, VS_TK, VS_CID)
