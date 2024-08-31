bin/jobExecutorServer 2021 2 5 &
sleep 1
bin/jobCommander linux08 2021 issueJob ls -l /usr/bin/* /usr/local/bin/* /bin/* /sbin/* /opt/* /etc/* /usr/sbin/* 
sleep 1
bin/jobCommander linux08 2021 exit