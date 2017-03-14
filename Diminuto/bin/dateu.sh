#!/bin/bash
# Emit current date in UTC in a format acceptable to set the date
# (perhaps on another machine).
date -u +%m%d%H%M%C%y.%S
