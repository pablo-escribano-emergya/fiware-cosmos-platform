# -*- coding: utf-8 -*-
"""
Variable expansion module.

"""
import re


class ExpansionContext(object):
    """
    Holds the values that can be expanded.
    """
    __NEXT_VARIABLE = re.compile('^(.*?)\$\{([^}]*)\}(.*)$', re.DOTALL)
    __DEFAULT_VALUES = {
        'job.id': 0,
        'job.name': 'jobname',
        'user.name': 'username',
        'user.home': '/user/username'
    }

    def __init__(self, job=None, user=None):
        """Creates an expansion context based on a job and user objects or
        default values when not provided."""
        self.__variables = self.__DEFAULT_VALUES.copy()
        if job is not None:
            self.__variables['job.id'] = job.id
            self.__variables['job.name'] = job.name
        if user is not None:
            self.__variables['user.name'] = user.username
            self.__variables['user.home'] = "/user/" + user.username

    def expand(self, text):
        """Expands variables in the form ${ var_name } in a given text."""
        remaining = text
        expanded = ''
        match = re.match(self.__NEXT_VARIABLE, remaining)
        while match is not None:
            prefix, variable, remaining = match.groups()
            expanded += prefix + self.__lookup(variable)
            match = re.match(self.__NEXT_VARIABLE, remaining)
        expanded += remaining
        return expanded

    def __lookup(self, variable):
        """Looks up a variable by name."""
        try:
            return unicode(self.__variables[variable.strip()])
        except KeyError:
            raise ValueError("Unknown varible to expand: '%s'" % variable)

    def decorate(self, fun):
        """
        Wraps fun to expand its only positional argument.
        """
        def decorated(text, **kwargs):
            return fun(self.expand(text), **kwargs)
        return decorated