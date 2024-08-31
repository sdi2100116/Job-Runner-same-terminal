bin/jobExecutorServer 2021 2 5 &
sleep 1
bin/jobCommander linux08 2021 setConcurrency 2
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &

#θα τρέξουν οι διεργασίες job_1,job_2 ταυτόχρονα και αφότου τελειώσουν θα ξεκινήσει να τρέχει η job_3