# -*- coding: utf-8 -*-
"""
Buildout to Jenkins adapter module.

"""
import os
import re
import sys
import subprocess


class BuildOut(object):
    """
    A BuildOut is a Python project that is based around a boostrap
    script and a buildout configuration file.
    """

    DEFAULT_CONFIG = 'buildout.devel.cfg'

    def __init__(self, project_name, config=DEFAULT_CONFIG):
        self.project = project_name
        self.config = config

    def build(self):
        """
        Bootstraps and then builds out a project
        """
        self.__cd_to_project()
        self.__run_subproc([sys.executable, 'bootstrap.py', '-c', self.config],
                           'bootstrapping')
        self.__run_subproc(['bin/buildout', '-c', self.config], 'building out')
        self.__cd_back()

    def run_tests(self):
        """
        Run NoseXUnit integrated tests
        """
        self.__cd_to_project()
        self.__run_subproc(['bin/hue', 'test', 'specific', self.project_name(),
                           '--with-nosexunit'], 'Unit testing')
        self.__cd_back()

    def project_name(self):
        return re.match(".*?([^/]*)/?$", self.project).group(1)

    def __cd_to_project(self):
        self.old_cwd = os.getcwd()
        if os.path.isdir(self.project):
            os.chdir(self.project)
        else:
            raise RuntimeException("The project dir %s was not found" %
                                   self.project)

    def __cd_back(self):
        os.chdir(self.old_cwd)

    def __run_subproc(self, command, desc):
        """
        Runs a command line string as a subprocess of this module. Requires a
        description of the subprocess for message formatting.
        """
        proc = subprocess.Popen(command, stderr=subprocess.STDOUT)
        proc.communicate()
        if proc.returncode != 0:
            print "Error while %s in project %s" % (desc, self.project)
        else:
            print "Success in %s project %s" % (desc, self.project)
