# -*- coding: utf-8 -*-
"""Create user command."""
from optparse import make_option

from django.contrib.auth.models import User
from django.core.management.base import BaseCommand, CommandError
from hadoop import cluster


class Command(BaseCommand):
    args = 'login'
    help = 'Register a new user to the cosmos platform'
    option_list = BaseCommand.option_list + (
        make_option('--password', action='store',
                    help='User password'),
        make_option('--email', action='store',
                    default='', help='User email address'),
        make_option('--first', action='store',
                    default='', help='First name'),
        make_option('--last', action='store',
                    default='', help='Family name'),
        make_option('--admin', help='Grant administrative privileges')
    )

    def handle(self, *args, **options):
        if len(args) != 1:
            raise CommandError("A single login was expected")
        login = args[0]
        if options.get('password', None) is None:
            raise CommandError("Please specify a password with --password")
        self.create_user(login, options)
        self.create_home(login)

    def create_user(self, login, options):
        try:
            User.objects.get(username=login)
            raise CommandError("User '%s' already exists" % login)
        except User.DoesNotExist:
            user = User.objects.create_user(login, options['email'],
                                             options['password'])
            user.first_name = options['first']
            user.last_name = options['last']
            if options.get('admin', False):
                user.is_superuser = True
                user.is_staff = True
            user.save()

    def create_home(self, login):
        home = '/user/%s' % login
        hdfs = cluster.get_hdfs()
        hdfs.setuser(login)
        for path in ['jars', 'datasets', 'tmp']:
            hdfs.mkdir('%s/%s' % (home, path))
