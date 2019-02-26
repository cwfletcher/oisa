require_rv64;
if(RS1)
    WRITE_RD(sext32(RS2));
else
    WRITE_RD(sext32(RD));

