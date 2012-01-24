try:
    import ez_setup
    ez_setup.use_setuptools()
except ImportError:
    pass

from setuptools import setup

setup(
    name='nose-iik-plugin',
    version='0.1',
    author='Andrew Tolbert',
    author_email = 'atolber@digi.com',
    description = 'Plugin to pass in iDigi Credentials and Device ID to IIK Tests.',
    license = 'For Digi Internal Use Only',
    py_modules = ['iik_plugin', 'build_plugin', 'build_utils',
                  'idigi_ws_api', 'configuration', 
                  'iik_testcase', 'utils', 'push_client'],
    entry_points = {
        'nose.plugins.0.10': [
            'iik = iik_plugin:IIKPlugin'
            ]
        }
    )