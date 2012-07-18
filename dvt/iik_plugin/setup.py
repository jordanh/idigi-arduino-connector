try:
    import ez_setup
    ez_setup.use_setuptools()
except ImportError:
    pass

from setuptools import setup

setup(
    name='nose-iik-plugin',
    version='0.9',
    author='Andrew Tolbert',
    author_email = 'atolber@digi.com',
    description = 'Plugin to pass in iDigi Credentials and Device ID to iDigi Connector Tests.',
    license = 'For Digi Internal Use Only',
    py_modules = ['iik_plugin', 'build_plugin', 'build_utils',
                  'idigi_ws_api', 'configuration', 
                  'iik_testcase', 'utils', 'push_client'],
    install_requires=['requests', 'py-dom-xpath'],
    entry_points = {
        'nose.plugins.0.10': [
            'iik = iik_plugin:IIKPlugin',
            'build = build_plugin:BuildPlugin',
            ]
        }
    )
