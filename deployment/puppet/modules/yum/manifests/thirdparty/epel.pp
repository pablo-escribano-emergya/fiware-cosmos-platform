#
# Telefónica Digital - Product Development and Innovation
#
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
#
# Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
# All rights reserved.
#

class yum::thirdparty::epel($repo_server = $yum::params::yum_epel) {

  include cosmos::params

  yumrepo { 'epel':
    descr    => "Extra Packages for Enterprise Linux ${::operatingsystemmajrelease}",
    enabled  => '1',
    gpgcheck => '0',
    baseurl  => $repo_server,
    proxy    => $cosmos::params::proxy
  }
}
