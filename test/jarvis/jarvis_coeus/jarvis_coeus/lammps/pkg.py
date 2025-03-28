"""
This module provides classes and methods to launch the Lammps application.
Lammps is ....
"""
from jarvis_cd.basic.pkg import Application
from jarvis_util import *
import os


class Lammps(Application):
    """
    This class provides methods to launch the Lammps application.
    """
    def _init(self):
        """
        Initialize paths
        """
        self.input_path = os.path.join(self.shared_dir, 'input.lammps')

    def _configure_menu(self):
        """
        Create a CLI menu for the configurator method.
        For thorough documentation of these parameters, view:
        https://github.com/scs-lab/jarvis-util/wiki/3.-Argument-Parsing

        :return: List(dict)
        """
        return [
            {
                'name': 'nprocs',
                'msg': 'Number of processes',
                'type': int,
                'default': 1,
            },
            {
                'name': 'ppn',
                'msg': 'The number of processes per node',
                'type': int,
                'default': 4,
            },
            {
                'name': 'engine',
                'msg': 'Engine to be used',
                'choices': ['bp4', 'hermes'],
                'type': str,
                'default': 'bp4',
            },
            {
                'name': 'script_location',
                'msg': 'the location of lammps script',  # the location for lammps scirpt
                'type': str,
                'default': None,

            },
            {
                'name': 'db_path',
                'msg': 'Path where the DB will be stored',
                'type': str,
                'default': 'benchmark_metadata.db',
            },
        ]

    def _configure(self, **kwargs):
        """
        Converts the Jarvis configuration to application-specific configuration.
        E.g., OrangeFS produces an orangefs.xml file.

        :param kwargs: Configuration parameters for this pkg.
        :return: None
        """
        # Get the complete script location
        self.config['full_script_location'] = os.path.expandvars(self.config['script_location'])
        if not os.path.exists(self.config['full_script_location']):
             self.config['full_script_location'] = os.path.join(self.pkg_dir, 'lammps-input-files/inputs', self.config['script_location'])
        if not os.path.exists(self.config['full_script_location']):
             self.log(f'Could not find the script: {self.config["script_location"]}', Color.RED)
             exit(1)
        # Change input.lammps to use adios
        with open(f'{self.config["full_script_location"]}/input.lammps', 'r') as f:
            lines = f.readlines()
        for i, line in enumerate(lines):
            if 'dump' in line and 'atom' in line and 'adios' not in line:
                lines[i] = line.replace('atom', 'atom/adios')
        with open(self.input_path, 'w') as f:
            f.writelines(lines)
        # Create the adios2_config.xml file
        if self.config['engine'].lower() == 'bp4':
            self.copy_template_file(f'{self.pkg_dir}/config/adios2.xml',
                                  f'{self.config["full_script_location"]}/adios2_config.xml')
        elif self.config['engine'].lower() == 'hermes':
            replacement = [("ppn", self.config['ppn']), ("DB_FILE", self.config['db_path'])]
            self.copy_template_file(f'{self.pkg_dir}/config/hermes.xml',
                                   f'{self.config["full_script_location"]}/adios2_config.xml', replacement)
        else:
           self.log(f'Engine not defined: {self.config["engine"]}', Color.RED)
           raise Exception('Engine not defined')

    def start(self):
        """
        Launch an application. E.g., OrangeFS will launch the servers, clients,
        and metadata services on all necessary pkgs.

        :return: None
        """
        engine_path = self.find_library('hermes_engine')
        if engine_path is None:
            self.log('Could not find hermes_engine', Color.RED)
            exit(1)
        self.env['ADIOS2_PLUGIN_PATH'] = os.path.dirname(engine_path)
        # self.env['ADIOS2_PLUGIN_PATH'] = '/home/llogan/Documents/Projects/coeus-adapter/build/bin'
        self.log(f'ADIOS2_PLUGIN_PATH: {self.env["ADIOS2_PLUGIN_PATH"]}', Color.YELLOW)
        Exec(f'lmp -in {self.input_path}',
             MpiExecInfo(nprocs=self.config['nprocs'],
                         ppn=self.config['ppn'],
                         hostfile=self.jarvis.hostfile,
                         env=self.env,
                         cwd=self.config['full_script_location'],
                         dbg_port=self.config['dbg_port'],
                         do_dbg=self.config['do_dbg']))
        pass

    def stop(self):
        """
        Stop a running application. E.g., OrangeFS will terminate the servers,
        clients, and metadata services.

        :return: None
        """
        pass

    def clean(self):
        """
        Destroy all data for an application. E.g., OrangeFS will delete all
        metadata and data directories in addition to the orangefs.xml file.

        :return: None
        """

        output_file = [self.config['db_path']]
        Rm(output_file, PsshExecInfo(hostfile=self.jarvis.hostfile))
        pass
