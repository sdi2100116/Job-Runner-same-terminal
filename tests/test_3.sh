#σε αυτο το test ελέγχεται αν ο buffer μπλοκάρεται σωστά
bin/jobExecutorServer 2021 1 5 &
sleep 1
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &
bin/jobCommander linux08 2021 issueJob bin/progDelay 10 &



#αφότου μπει η διεργασία job_2 στον buffer μπλοκάρει και εμφανίζει μήνυμα ο server. Αμέσως μόλις ξεκινήσει να τρέχει η διεργασία job_2 τότε ο buffer
#ξεμπλοκάρει (μήνυμα server:Buffer unblocked...) και εισάγεται η διεργασία job_3 και τρέχει αφότου τελειώσει η job_2, αφότου ξεκινήσει να τρέχει η job_3 
#εισάγεται και η διεργασια job_4 και τρέχει οταν τελειώσει η job_3