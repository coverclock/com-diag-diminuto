#!/bin/bash
free -h
ls -l /var/swap
sudo /sbin/dphys-swapfile swapoff
sudo ${EDITOR:-"vi"} /etc/dphys-swapfile # I used CONF_SWAPSIZE=2048
sudo /sbin/dphys-swapfile setup
sudo /sbin/dphys-swapfile swapon
free -h
ls -l /var/swap
