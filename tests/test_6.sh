#το test αυτό ελέγχει την περίπτωση που το Concurrency αυξάνεται κατα τη διάρκεια που τρέχει ο σερβερ
bin/jobExecutorServer 2021 5 5 &
sleep 1
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
sleep 1
bin/jobCommander linux08 2021 issueJob ls &
bin/jobCommander linux08 2021 issueJob ls &
sleep 1
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
sleep 1
bin/jobCommander linux08 2021 setConcurrency 2


#οταν το τρέξουμε παρατηρούμε οτι τρέχει αρχικά η διεργασία 1,εισάγονται οι διεργασίες 2,3,4 και το Concurrency γίνεται 2, οπότε ξεκινάει να τρέχει η 
#διεργασία 2 παράλληλα με την 1 και τελείωνει πολύ γρήγορα και ξεκινάει η 3 και τελειώνει και αυτή. Μόλις τελειώσει και η 3 ξεκινάει η 4.