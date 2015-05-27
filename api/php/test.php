<?php
require_once(dirname(__FILE__) . '/sim.php');

$ip = '127.0.0.1';
$port = 8800;
$sim = new SimClient($ip, $port, -1);

$req = array(
	'echo',
	'你好',
	'hello world!',
	'[hi]',
	array(
		'a',
		'中',
		'国',
		'b',
	),
);
$sim->send($req);
$resp = $sim->recv();
var_dump($resp);

$total = 10000;
$stime = microtime(1);
for($i=0; $i<$total; $i++){
	$s = sim_encode($req);
}
$speed = $total / (microtime(1) - $stime);
printf("encode speed: %d /s\n", $speed);


$total = 10000;
$stime = microtime(1);
for($i=0; $i<$total; $i++){
	$sim->send($req);
	$resp = $sim->recv();
}
$speed = $total / (microtime(1) - $stime);
printf("speed: %d qps\n", $speed);

