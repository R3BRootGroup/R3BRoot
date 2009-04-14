void ed()
{
   TEveManager::Create();

   gGeoManager = gEve->GetGeometry("r3bgeo.root");
   gGeoManager->DefaultColors();

   TGeoVolume* top = gGeoManager->GetTopVolume();;

   TEveGeoNode* cell = new TEveGeoNode(gGeoManager, top->FindNode("cave_1"));
   cell->SetVisLevel(6);
   gEve->AddGlobalElement(cell);

/*   TEveGeoTopNode* target = new TEveGeoTopNode(gGeoManager, top->FindNode("leadTarget_log_0"));
   target->SetVisLevel(6);
   gEve->AddGlobalElement(target);

   TEveGeoTopNode* calo = new TEveGeoTopNode(gGeoManager, top->FindNode("CBLogWorld_0"));
   calo->SetVisLevel(6);
   gEve->AddGlobalElement(calo);
*/
   gEve->FullRedraw3D(kTRUE);

   // EClipType not exported to CINT (see TGLUtil.h):
   // 0 - no clip, 1 - clip plane, 2 - clip box
   gEve->GetGLViewer()->GetClipSet()->SetClipType(1);
   gEve->GetGLViewer()->SetClearColor(kBlue-10);
   gEve->GetGLViewer()->SetGuideState(TGLUtil::kAxesEdge, kTRUE, kFALSE, 0);
   gEve->GetGLViewer()->RefreshPadEditor(gEve->GetGLViewer());
}