-- CAIO command result strings
REPLACE INTO `trinity_string` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`)
VALUES (60002, 'Force reload message sent to %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
REPLACE INTO `trinity_string` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`)
VALUES (60003, 'Force reset message sent to %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
REPLACE INTO `trinity_string` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`)
VALUES (60004, 'There was a problem reloading client addons. Force reload was not sent.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
REPLACE INTO `trinity_string` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`)
VALUES (60005, 'Addon with name \'%s\' already exists or file not found.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
REPLACE INTO `trinity_string` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`)
VALUES (60006, 'Addon with name \'%s\' not found.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

-- CAIO commands
REPLACE INTO `command` (`name`, `permission`, `help`) VALUES ('caio', 5000, 'Syntax: .caio $subcommand\nType .caio to see the list of possible subcommands or .help caio $subcommand to see info on subcommands');
REPLACE INTO `command` (`name`, `permission`, `help`) VALUES ('caio version', 5000, 'Syntax: .caio version\nShows the AIO version');
REPLACE INTO `command` (`name`, `permission`, `help`) VALUES ('caio addaddon', 5000, 'Syntax: .caio addaddon $addonName [$permission] "$addonFile"\nAdds an addon to addon list for players with $permission and force reloads all affected player addons');
REPLACE INTO `command` (`name`, `permission`, `help`) VALUES ('caio removeaddon', 5000, 'Syntax: .caio removeaddon $addonName\nRemoves an addon from addon list and force reloads all affected player addons');
REPLACE INTO `command` (`name`, `permission`, `help`) VALUES ('caio reloadaddons', 5000, 'Syntax: .caio reloadaddons\nReloads all client addons loaded on the server and forces reload on all players.');
REPLACE INTO `command` (`name`, `permission`, `help`) VALUES ('caio forcereload', 5000, 'Syntax: .caio forcereload $playerName\nForce reloads player\'s addons. Player addons are synced with server.');
REPLACE INTO `command` (`name`, `permission`, `help`) VALUES ('caio forcereset', 5000, 'Syntax: .caio forcereset $playerName\nForce resets player\'s addons. Player addons are deleted and downloaded again.');
REPLACE INTO `command` (`name`, `permission`, `help`) VALUES ('caio forcereloadall', 5000, 'Syntax: .caio forcereloadall [$permission]\nForce reloads players of $permission and above. Affected players\' addons are synced with the server.');
REPLACE INTO `command` (`name`, `permission`, `help`) VALUES ('caio forceresetall', 5000, 'Syntax: .caio forceresetall [$permission]\nForce resets players of $permission and above. Affected players\' addons are deleted and downloaded again.');
REPLACE INTO `command` (`name`, `permission`, `help`) VALUES ('caio send', 5000, 'Syntax: .caio send $playerName "Message"\nSends an addon message to player');
REPLACE INTO `command` (`name`, `permission`, `help`) VALUES ('caio sendall', 5000, 'Syntax: .caio sendall [$permission] "Message"\nSends an addon message to all players of $permission and above');
