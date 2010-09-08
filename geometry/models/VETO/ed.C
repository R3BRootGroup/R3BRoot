void ed()
{
   TEveManager::Create();

   gGeoManager = gEve->GetGeometry("veto.root");
   gGeoManager->DefaultColors();
   cout<<"get top volume"<<endl;
   TGeoVolume* top = gGeoManager->GetTopVolume();;
   cout<<"volume get!"<<endl;
   TEveGeoTopNode* cell = new TEveGeoTopNode(gGeoManager, top->FindNode("VETOLog_1"));
   cell->SetVisLevel(6);
   gEve->AddGlobalElement(cell);
/*
   TEveGeoTopNode* calo = new TEveGeoTopNode(gGeoManager, top->FindNode("CALO_1"));
   calo->SetVisLevel(3);
   gEve->AddGlobalElement(calo);

   TEveGeoTopNode* muon = new TEveGeoTopNode(gGeoManager, top->FindNode("MUON_1"));
   muon->SetVisLevel(4);
   gEve->AddGlobalElement(muon);
*/
   gEve->FullRedraw3D(kTRUE);

   // EClipType not exported to CINT (see TGLUtil.h):
   // 0 - no clip, 1 - clip plane, 2 - clip box
   gEve->GetGLViewer()->GetClipSet()->SetClipType(1);
   gEve->GetGLViewer()->SetClearColor(kBlue-10);
   gEve->GetGLViewer()->SetGuideState(TGLUtil::kAxesEdge, kTRUE, kFALSE, 0);
   gEve->GetGLViewer()->RefreshPadEditor(gEve->GetGLViewer());
}
