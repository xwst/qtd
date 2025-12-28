INSERT INTO tags VALUES('0f066904-a88a-4fb3-a098-19594a6135dd','Private',    '#333333',NULL);
INSERT INTO tags VALUES('fcff6021-2d4c-46df-92bd-6cabe0ae75b1','Fun',        '#FFFF00','0f066904-a88a-4fb3-a098-19594a6135dd');
INSERT INTO tags VALUES('3fd213cf-1c50-47a0-9237-c2da78bf4fcc','Vacation',   '#FF0000','fcff6021-2d4c-46df-92bd-6cabe0ae75b1');
INSERT INTO tags VALUES('10173aba-edd8-4049-a41c-74f28581c31f','Hobbies',    '#00FF00','fcff6021-2d4c-46df-92bd-6cabe0ae75b1');
INSERT INTO tags VALUES('18a2d601-712e-4ac4-b655-93c5a288dc99','Chores',     '#559999','0f066904-a88a-4fb3-a098-19594a6135dd');
INSERT INTO tags VALUES('54c1f21d-bb9a-41df-9658-5111e153f745','Shopping',   '#AA9999','0f066904-a88a-4fb3-a098-19594a6135dd');
INSERT INTO tags VALUES('9e711519-7fe7-487b-8f44-e2a31e5ed1e7','Finance',    '',       '0f066904-a88a-4fb3-a098-19594a6135dd');
INSERT INTO tags VALUES('c99586cb-3910-4fab-b5a4-d936c9e58471','Work',       '#AAAAAA',NULL);
INSERT INTO tags VALUES('b7f5d20c-3ea7-4d20-86e2-3c682fc05756','Mails',      '#FF5555','c99586cb-3910-4fab-b5a4-d936c9e58471');
INSERT INTO tags VALUES('0baf3308-5899-44ad-9e55-a8e83f2b82ee','Spreadsheet','#AA5555','c99586cb-3910-4fab-b5a4-d936c9e58471');

INSERT INTO tasks VALUES('84723285-3f82-a463-3274-d65afa4bafc9','Cook meal',            'open',  '2025-12-01 16:00:00','2025-12-01 18:15:00',''                   ,'');
INSERT INTO tasks VALUES('dc1f5ff8-db45-6630-9340-13a7d860d910','Buy groceries',        'open',  '',                   '2025-12-01 16:00:00',''                   ,'Also check if toothpaste is empty');
INSERT INTO tasks VALUES('a2225a6f-10ab-2f2a-b7be-e97637804d1b','Print recipe',         'open',  '',                   '2025-12-01 16:00:00',''                   ,'');
INSERT INTO tasks VALUES('3d41278f-4130-a2ae-383b-cbfe2c45f4b5','Print shopping list',  'open',  '',                   '2025-12-01 16:00:00',''                   ,'');
INSERT INTO tasks VALUES('ff7cebda-eef6-a632-99e4-1678b69758e7','Check food supplies',  'closed','2024-11-30 00:00:00',''                   ,'2024-11-29 12:17:07','');
INSERT INTO tasks VALUES('0128dd5a-79a9-4228-b211-fa1724b8d149','Do chores',            'closed','',                   ''                   ,'2024-11-29 16:58:44',REPLACE('- Clean bathroom\n- Sweep\n- …','\n',char(10)));
INSERT INTO tasks VALUES('125801f4-edc2-5e16-0b66-3e128098a5e5','Answer landlords mail','open',  '',                   '2025-11-30 23:59:59',''                   ,'');
INSERT INTO tasks VALUES('0cdaf3ea-9544-71e5-545c-8b5dec226f86','Fix printer',          'open',  '',                   ''                   ,''                   ,'');

INSERT INTO tag_assignments VALUES('84723285-3f82-a463-3274-d65afa4bafc9', '10173aba-edd8-4049-a41c-74f28581c31f');
INSERT INTO tag_assignments VALUES('dc1f5ff8-db45-6630-9340-13a7d860d910', '54c1f21d-bb9a-41df-9658-5111e153f745');
INSERT INTO tag_assignments VALUES('ff7cebda-eef6-a632-99e4-1678b69758e7', '54c1f21d-bb9a-41df-9658-5111e153f745');
INSERT INTO tag_assignments VALUES('0128dd5a-79a9-4228-b211-fa1724b8d149', '18a2d601-712e-4ac4-b655-93c5a288dc99');

INSERT INTO dependencies VALUES('84723285-3f82-a463-3274-d65afa4bafc9','dc1f5ff8-db45-6630-9340-13a7d860d910');
INSERT INTO dependencies VALUES('84723285-3f82-a463-3274-d65afa4bafc9','a2225a6f-10ab-2f2a-b7be-e97637804d1b');
INSERT INTO dependencies VALUES('a2225a6f-10ab-2f2a-b7be-e97637804d1b','0cdaf3ea-9544-71e5-545c-8b5dec226f86');
INSERT INTO dependencies VALUES('3d41278f-4130-a2ae-383b-cbfe2c45f4b5','0cdaf3ea-9544-71e5-545c-8b5dec226f86');
INSERT INTO dependencies VALUES('125801f4-edc2-5e16-0b66-3e128098a5e5','0cdaf3ea-9544-71e5-545c-8b5dec226f86');
INSERT INTO dependencies VALUES('dc1f5ff8-db45-6630-9340-13a7d860d910','3d41278f-4130-a2ae-383b-cbfe2c45f4b5');
INSERT INTO dependencies VALUES('3d41278f-4130-a2ae-383b-cbfe2c45f4b5','ff7cebda-eef6-a632-99e4-1678b69758e7');
