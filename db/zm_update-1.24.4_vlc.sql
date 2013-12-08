alter table Monitors modify column `Type` enum('Local','Remote','File','Ffmpeg','Vlc') NOT NULL DEFAULT 'Local';

