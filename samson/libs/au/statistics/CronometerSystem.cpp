/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

#include "CronometerSystem.h"  // Own interface
#include "au/string/StringUtilities.h"

namespace au {
CronometerSystem::CronometerSystem() {
  running_ = false;
  seconds_ = 0;
}

void CronometerSystem::Start() {
  running_ = true;
  cronometer_.Reset();
}

void CronometerSystem::Stop() {
  running_ = false;
  seconds_ += cronometer_.seconds();
}

// Reset the count to 0
void CronometerSystem::Reset() {
  running_ = false;
  seconds_ = 0;
}

double CronometerSystem::running_seconds() const {
  if (running_) {
    return cronometer_.seconds();
  } else {
    return 0;
  }
}

double CronometerSystem::seconds() const {
  if (running_) {
    return (seconds_ + cronometer_.seconds());
  } else {
    return seconds_;
  }
}
  
  std::string CronometerSystem::str_seconds() const
  {
    return  au::str_time( seconds() );
  }

std::ostream& operator<<(std::ostream& o, const CronometerSystem& cronometer_system) {
  o << "[ " << au::str_time_simple(cronometer_system.seconds());
  if (cronometer_system.running_) {
    o << " running for " << cronometer_system.cronometer_;
  }
  o << " ]";
  return o;
}
}