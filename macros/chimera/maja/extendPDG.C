void extendPDG(){
  TDatabasePDG*db=TDatabasePDG::Instance();
  db->ReadPDGTable("plist.in");
  db->WritePDGTable("plist.out");
}
