#το test αυτό ελέγχει την περίπτωση που το Concurrency μειώνεται κατα τη διάρκεια που τρέχει ο σερβερ
bin/jobExecutorServer 2021 5 5 &
sleep 1
bin/jobCommander linux08 2021 setConcurrency 3
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
sleep 1
bin/jobCommander linux08 2021 setConcurrency 1


#όταν τρέξουμε αυτό το test αρχικά ξεκινάνε οι 3 πρώτες διεργασίες να τρέχουν ταυτόχρονα και τελειώνουν ταυτόχρονα,μετά το Concurrency αλλάζει σε 1 
#επομένως οι δύο επόμενες διεργασίες τρέχουν μία-μία με διαφορά 10 δεπτερολέπτων μεταξύ τους.(progDelay 10)