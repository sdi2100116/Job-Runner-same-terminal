#σε αυτό το test δοκιμάζεται η exit
bin/jobExecutorServer 2021 5 5 &
sleep 1
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
sleep 1
bin/jobCommander linux08 2021 exit

#όταν τρέξει αυτό το test βλέπουμε οτι επιστρέφεται 4 φορές το SERVER TERMINATED BEFORE EXECUTION για τις διεργασίες απο τη δευτερη και μετά που δεν
#τρέχουν αυτή τη στιγμή. Η διεργασία 1 που έχει ηδη ξεκινήσει να τρέχει επιστρέφει κανονικά. Όταν τελειώσει επιστρέφεται SERVER TERMINATED και ο 
#server τερματίζει