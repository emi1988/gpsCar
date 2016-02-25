<?php

$timeStamp = $_POST['timeStamp'];
$longitudeDecimal = $_POST['longitudeDecimal'];
$latitudeDecimal   = $_POST['latitudeDecimal'];
$altitude = $_POST['altitude'];
$satelliteAmount   = $_POST['satelliteAmount'];
$horizontalPrecision   = $_POST['horizontalPrecision'];

//user data removed!!

define ( 'MYSQL_HOST',      'localhost' );
define ( 'MYSQL_BENUTZER',  '' );
define ( 'MYSQL_KENNWORT',  '' );
define ( 'MYSQL_DATENBANK', '' );


$db_link = mysql_connect (MYSQL_HOST, MYSQL_BENUTZER, MYSQL_KENNWORT);


$db_sel = mysql_select_db( MYSQL_DATENBANK )
or die("Auswahl der Datenbank fehlgeschlagen");


//$sql = "SELECT * FROM position";        //daten ist tabelle

//$db_erg = mysql_query( $sql );

$eintrag = "INSERT INTO raspiGPScar
(timeStamp, longitudeDecimal, latitudeDecimal, altitude, satelliteAmount, horizontalPrecision)
VALUES
('$timeStamp', '$longitudeDecimal', '$latitudeDecimal', '$altitude', '$satelliteAmount', '$horizontalPrecision')";

$eintragen = mysql_query($eintrag);

mysql_free_result( $db_erg );



if($eintragen == true)
   {
echo '<b>';   
echo "Eintrag war erfolgreich";
echo '</b>';
   }
else
   {
   echo "Fehler beim Speichern";
   }



mysql_close($db_link );



?>