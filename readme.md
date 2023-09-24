# AutoRouter2

Find out non-ACKed TCP syn request packet in realtime network traffic and add the destination IP address into a ipset.


## principle

If `TCP SYN` sent to an ip, but `TCP ACK` not received in `syn_expire` seconds, the ip will add to `ipset_name` ipset.

Added ip will remove after `ack_expire` seconds.


## build and install

```shell
apt-get install libipset-dev libhiredis-dev libpcap-dev -y    # install libs
make    # compile application
cp build/auto-router2 /root/auto-router2/    # copy executable to /root/auto-router/
cp auto-router2.service /etc/systemd/system/    # copy systemd service config file
```

## setup redis server

Uncomment `notify-keyspace-events` in redis server config file and set the value to `"Ex"`.

Replace `bind 127.0.0.1 ::1` to `0.0.0.0 ::` if required.


## configuration

**auto-router.service**

Change `ExecStart`, `WorkingDirectory`, and other if need.

Program arguments:
+ -h redis_server_host
+ -p redis_server_port
+ -i sniffer_outbound_interface_name
+ -s syn_expire_seconds
+ -a ack_expire_seconds
+ -l ipset_name


## reload and start service

```
systemctl daemon-reload
systemctl enable --now auto-router2.service
ps -ef | grep auto-router2
```
