#!/bin/sh

# Utility script to clean up sandbox before checkin
svn delete cosmos.kdevses --force
svn delete debug/
