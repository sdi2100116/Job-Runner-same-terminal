#σε αυτό το test ελέγχεται η εντολή poll και αν λειτουργεί η εντολή stop ενώ το job υπάρχει 
bin/jobExecutorServer 2021 5 5 &
sleep 1
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
sleep 1
bin/jobCommander linux08 2021 poll
sleep 1
bin/jobCommander linux08 2021 stop job_3
sleep 1
bin/jobCommander linux08 2021 poll

#οταν τρέξει το test στην πρώτη poll πρεπει να εμφανιστούν job 2,3,4,5,6 αμέσως μετά JOB job_3 REMOVED και στη δεύτερη poll job 2,4,5,6