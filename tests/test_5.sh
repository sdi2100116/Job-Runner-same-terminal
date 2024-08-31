#σε αυτό το test ελέγχεται αν λειτουργεί η εντολή stop ενώ το job δεν υπάρχει
bin/jobExecutorServer 2021 5 5 &
sleep 1
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
sleep 3 #sleep ωστε η poll να τρέξει εφόσον έχουν εισαχθεί όλα τα jobs μιας και δεν ειναι απαραίτητο οτι οι εντολές θα τρέξουν με τη σειρα
bin/jobCommander linux08 2021 poll
sleep 1
bin/jobCommander linux08 2021 stop job_8
sleep 1
bin/jobCommander linux08 2021 poll

#otan τρέξει αυτό το test η πρώτη poll θα εμφανίσει job 2,3,4,5,6, μετά θα εμφανιστεί  JOB job_8 NOTFOUND και η επόμενο poll θα εμφανίσει πάλι job 2,3,4,5,6