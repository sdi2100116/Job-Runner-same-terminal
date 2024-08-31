#το test αυτό ελέγχει την exit όταν jobs περιμένουν για να μπούν στον buffer
bin/jobExecutorServer 2021 1 5 &
sleep 1
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
sleep 1
bin/jobCommander linux08 2021 exit

#oταν τρέξει αυτό το test πρέπει να εμφανίσει 3 φορες το SERVER TERMINATED BEFORE EXECUTION (1 φορά για το job_2 που είναι στον buffer και 2 φορές 
#για τα jobs 3 και 4 που περιμένουν να ανοίξει κενή θέση στον buffer για να μπουν). Να εμφανιστεί το αποτέλεσμα της διεργασίας 1 που τρέχει και 
#μολις τελειώσει να εμφανιστεί SERVER TERMINATED
