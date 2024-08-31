#σε αυτό το test ελέγχεται άν λειτουργεί το issueJob και αν το workerThread εκτελεί σωστά τις λειτουργίες του
bin/jobExecutorServer 2021 2 5 &
sleep 1
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 poll 

#το poll πρεπει να επιστρεψει job_2,job_3 και να τρέξουν οι διεργασίες job_1,job_2,job_3 με 10 δεπτερόλεπτα διαφορά η κάθε μία 
#αν κανούμε και ls στο τέλος αφότου τελειώσουν οι διεργασίες μπορούμε να δούμε οτι έχουν διαγραφέι τα pid.output