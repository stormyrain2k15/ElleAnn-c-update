<?php

        header('Content-Type: text/html');
        header("Cache-Control: no-cache, must-revalidate"); // HTTP/1.1
        header("Expires: Sat, 26 Jul 1997 05:00:00 GMT"); // Date in the past
        header("Pragma: no-cache"); // Date in the past

	function printSuccess($reqid)
    {
        print "<?xml version='1.0' encoding='UTF-8'?>";
        print "<ROOT>";
        print "<AuthenticationInfo>";
        print "<result>SUCCESS</result>";
        print "<requestid>" . $reqid . "</requestid>";
        print "<cpid>1</cpid>";
        print "<birthdate>1/1/1900</birthdate>";
        print "<gender> </gender>";
        print "<countrycode>A1</countrycode>";
        print "</AuthenticationInfo>";
        print "</ROOT>";
    }

	if(isset($_GET['User']) && isset($_GET['Password'])){
    	if($_GET['User'] == 'Klurr' || $_GET['User'] == 'Miff' || $_GET['User'] == 'Muhment')
        {
    	   if($_GET['Password'] == '09944cb770631d1bcfa738cb6ea2f0a1') //SALT + PASS 'dlrtkdlxm!' + 'omglolpass'
           {
    	       printSuccess($_GET['User']);
           }
        }
    }

?>