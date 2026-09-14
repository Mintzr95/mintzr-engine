package com.mju.engine;

import android.app.Activity;
import android.os.Bundle;
import android.graphics.*;
import android.graphics.drawable.GradientDrawable;
import android.view.*;
import android.widget.*;
import java.io.*;
import java.util.*;

public class EditorActivity extends Activity {
  EditorCanvas canvas; TextView inspectorText; LinearLayout hierarchy; EditText nameEdit,xEdit,yEdit; CheckBox visibleBox,lockedBox; SeekBar zoom;
  @Override public void onCreate(Bundle b){super.onCreate(b); buildUi();}
  TextView label(String s){ TextView t=new TextView(this); t.setText(s); t.setTextColor(Color.WHITE); t.setGravity(Gravity.CENTER_VERTICAL|Gravity.RIGHT); t.setPadding(12,0,12,0); return t; }
  Button action(String s){ Button v=new Button(this); v.setText(s); v.setTextColor(Color.WHITE); v.setAllCaps(false); v.setMinHeight(0); v.setPadding(12,0,12,0); GradientDrawable g=new GradientDrawable(); g.setColor(Color.rgb(35,45,60)); g.setCornerRadius(18); v.setBackground(g); return v; }
  EditText field(String hint){ EditText e=new EditText(this); e.setHint(hint); e.setTextColor(Color.WHITE); e.setHintTextColor(Color.rgb(150,160,175)); e.setSingleLine(true); e.setPadding(12,0,12,0); return e; }
  void buildUi(){
    LinearLayout root=new LinearLayout(this); root.setOrientation(LinearLayout.VERTICAL); root.setBackgroundColor(Color.rgb(13,17,24));
    LinearLayout bar=new LinearLayout(this); bar.setGravity(Gravity.CENTER_VERTICAL); bar.setPadding(8,8,8,8);
    TextView title=label("MJU  •  محرر اللعبة"); title.setTextSize(18); bar.addView(title,new LinearLayout.LayoutParams(0,56,1));
    Button add=action("+ عنصر"), dup=action("نسخ"), del=action("حذف"), undo=action("تراجع"), redo=action("إعادة"), save=action("حفظ"), play=action("تشغيل");
    for(Button b:new Button[]{add,dup,del,undo,redo,save,play}) bar.addView(b,new LinearLayout.LayoutParams(-2,56)); root.addView(bar);
    LinearLayout body=new LinearLayout(this); body.setOrientation(LinearLayout.HORIZONTAL);

    LinearLayout tree=new LinearLayout(this); tree.setOrientation(LinearLayout.VERTICAL); tree.setBackgroundColor(Color.rgb(21,26,35));
    TextView th=label("المشهد / Hierarchy"); th.setTextSize(15); tree.addView(th,new LinearLayout.LayoutParams(220,54));
    ScrollView sv=new ScrollView(this); hierarchy=new LinearLayout(this); hierarchy.setOrientation(LinearLayout.VERTICAL); sv.addView(hierarchy); tree.addView(sv,new LinearLayout.LayoutParams(220,0,1));
    body.addView(tree,new LinearLayout.LayoutParams(220,-1));

    FrameLayout canvasHolder=new FrameLayout(this); canvas=new EditorCanvas(); canvasHolder.addView(canvas,new FrameLayout.LayoutParams(-1,-1));
    LinearLayout zoomBar=new LinearLayout(this); zoomBar.setPadding(10,6,10,6); zoomBar.setGravity(Gravity.CENTER_VERTICAL);
    TextView zl=label("تكبير"); zl.setTextSize(12); zoomBar.addView(zl,new LinearLayout.LayoutParams(70,44)); zoom=new SeekBar(this); zoom.setMax(200); zoom.setProgress(100); zoomBar.addView(zoom,new LinearLayout.LayoutParams(180,44));
    FrameLayout.LayoutParams zlp=new FrameLayout.LayoutParams(260,56,Gravity.TOP|Gravity.RIGHT); canvasHolder.addView(zoomBar,zlp); body.addView(canvasHolder,new LinearLayout.LayoutParams(0,-1,1));

    LinearLayout inspector=new LinearLayout(this); inspector.setOrientation(LinearLayout.VERTICAL); inspector.setPadding(12,12,12,12); inspector.setBackgroundColor(Color.rgb(23,29,39));
    TextView ih=label("Inspector"); ih.setTextSize(16); inspector.addView(ih,new LinearLayout.LayoutParams(260,50));
    inspectorText=label("لا يوجد تحديد"); inspectorText.setTextSize(13); inspector.addView(inspectorText,new LinearLayout.LayoutParams(260,60));
    nameEdit=field("اسم العنصر"); inspector.addView(nameEdit,new LinearLayout.LayoutParams(260,48));
    LinearLayout xy=new LinearLayout(this); xEdit=field("X"); yEdit=field("Y"); xy.addView(xEdit,new LinearLayout.LayoutParams(0,48,1)); xy.addView(yEdit,new LinearLayout.LayoutParams(0,48,1)); inspector.addView(xy,new LinearLayout.LayoutParams(260,52));
    visibleBox=new CheckBox(this); visibleBox.setText("مرئي"); visibleBox.setTextColor(Color.WHITE); inspector.addView(visibleBox,new LinearLayout.LayoutParams(260,46));
    lockedBox=new CheckBox(this); lockedBox.setText("مقفل"); lockedBox.setTextColor(Color.WHITE); inspector.addView(lockedBox,new LinearLayout.LayoutParams(260,46));
    TextView help=label("اسحب العنصر في الـCanvas. التعديل من Inspector يطبق مباشرة."); help.setGravity(Gravity.TOP|Gravity.RIGHT); help.setTextSize(12); inspector.addView(help,new LinearLayout.LayoutParams(260,-1));
    body.addView(inspector,new LinearLayout.LayoutParams(260,-1)); root.addView(body,new LinearLayout.LayoutParams(-1,0,1));
    setContentView(root);

    add.setOnClickListener(v->{canvas.addBox();refreshAll();}); dup.setOnClickListener(v->{canvas.duplicate();refreshAll();}); del.setOnClickListener(v->{canvas.deleteSelected();refreshAll();});
    undo.setOnClickListener(v->{canvas.undo();refreshAll();}); redo.setOnClickListener(v->{canvas.redo();refreshAll();}); save.setOnClickListener(v->{canvas.saveScene();toast("تم حفظ المشهد");}); play.setOnClickListener(v->{ startActivity(new android.content.Intent(this,MainActivity.class)); });
    zoom.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener(){public void onProgressChanged(SeekBar s,int p,boolean u){canvas.zoom=0.5f+p/100f;canvas.invalidate();}public void onStartTrackingTouch(SeekBar s){}public void onStopTrackingTouch(SeekBar s){}});
    View.OnFocusChangeListener fl=(v,has)->{if(!has) applyInspector();}; nameEdit.setOnFocusChangeListener(fl); xEdit.setOnFocusChangeListener(fl); yEdit.setOnFocusChangeListener(fl);
    visibleBox.setOnClickListener(v->applyInspector()); lockedBox.setOnClickListener(v->applyInspector());
    canvas.listener=()->refreshAll(); refreshAll();
  }
  void refreshAll(){refreshHierarchy();refreshInspector();}
  void refreshHierarchy(){hierarchy.removeAllViews(); for(int i=0;i<canvas.nodes.size();i++){final int idx=i; Button b=action((i==canvas.selected?"◆ ":"• ")+canvas.nodes.get(i).name); b.setGravity(Gravity.RIGHT); b.setOnClickListener(v->{canvas.selected=idx;canvas.invalidate();refreshInspector();refreshHierarchy();}); hierarchy.addView(b,new LinearLayout.LayoutParams(-1,48));}}
  void refreshInspector(){ if(canvas.selected<0||canvas.selected>=canvas.nodes.size()){inspectorText.setText("لا يوجد تحديد"); nameEdit.setText("");xEdit.setText("");yEdit.setText("");return;} Node n=canvas.nodes.get(canvas.selected); inspectorText.setText("ID: "+n.id+"\nSprite: "+n.texture+"\nLayer: "+n.layer); nameEdit.setText(n.name);xEdit.setText(String.valueOf(Math.round(n.x)));yEdit.setText(String.valueOf(Math.round(n.y)));visibleBox.setChecked(n.visible);lockedBox.setChecked(n.locked); }
  void applyInspector(){if(canvas.selected<0||canvas.selected>=canvas.nodes.size())return; Node n=canvas.nodes.get(canvas.selected); n.name=nameEdit.getText().toString().trim().isEmpty()?"Node":nameEdit.getText().toString().trim(); try{n.x=Float.parseFloat(xEdit.getText().toString());}catch(Exception ignored){} try{n.y=Float.parseFloat(yEdit.getText().toString());}catch(Exception ignored){} n.visible=visibleBox.isChecked(); n.locked=lockedBox.isChecked(); canvas.invalidate();refreshHierarchy();refreshInspector();}
  void toast(String s){Toast.makeText(this,s,Toast.LENGTH_SHORT).show();}

  class EditorCanvas extends View {
    Paint p=new Paint(Paint.ANTI_ALIAS_FLAG); ArrayList<Node> nodes=new ArrayList<>(); ArrayDeque<ArrayList<Node>> undo=new ArrayDeque<>(), redo=new ArrayDeque<>(); int selected=-1; float sx,sy; boolean dragging; float zoom=1f; Runnable listener; int nextId=1;
    EditorCanvas(){super(EditorActivity.this); setFocusable(true); addBox(); }
    ArrayList<Node> snap(){ArrayList<Node> c=new ArrayList<>();for(Node n:nodes)c.add(new Node(n));return c;} void checkpoint(){undo.push(snap());while(undo.size()>30)undo.removeLast();redo.clear();}
    void addBox(){checkpoint();nodes.add(new Node(nextId++,"Node"+nodes.size(),140+nodes.size()*42,150+nodes.size()*42));selected=nodes.size()-1;invalidate();}
    void duplicate(){if(selected<0)return;checkpoint();Node n=nodes.get(selected);nodes.add(new Node(nextId++,n.name+" Copy",n.x+36,n.y+36));selected=nodes.size()-1;invalidate();}
    void deleteSelected(){if(selected<0)return;checkpoint();nodes.remove(selected);selected=Math.min(selected,nodes.size()-1);invalidate();}
    void undo(){if(undo.isEmpty())return;redo.push(snap());nodes=undo.pop();selected=nodes.isEmpty()?-1:Math.min(selected,nodes.size()-1);invalidate();}
    void redo(){if(redo.isEmpty())return;undo.push(snap());nodes=redo.pop();selected=nodes.isEmpty()?-1:Math.min(selected,nodes.size()-1);invalidate();}
    void saveScene(){try{File d=new File(getFilesDir(),"project");d.mkdirs();File f=new File(d,"main.mju");BufferedWriter w=new BufferedWriter(new FileWriter(f));w.write("MJU_EDITOR 2\n");for(Node n:nodes)w.write(n.id+"|"+n.name.replace("|","_")+"|"+n.x+"|"+n.y+"|"+n.visible+"|"+n.locked+"|"+n.texture+"\n");w.close();}catch(Exception ignored){}}
    @Override protected void onDraw(Canvas c){super.onDraw(c);p.setStyle(Paint.Style.FILL);p.setColor(Color.rgb(15,20,28));c.drawRect(0,0,getWidth(),getHeight(),p);p.setStrokeWidth(1);p.setColor(Color.rgb(36,44,57));float step=32*zoom;for(float x=0;x<getWidth();x+=step)c.drawLine(x,0,x,getHeight(),p);for(float y=0;y<getHeight();y+=step)c.drawLine(0,y,getWidth(),y,p);p.setStyle(Paint.Style.STROKE);p.setStrokeWidth(2);p.setColor(Color.rgb(70,82,102));c.drawRect(30,30,Math.min(getWidth()-30,900),Math.min(getHeight()-30,600),p);for(int i=0;i<nodes.size();i++){Node n=nodes.get(i);if(!n.visible)continue;float size=48*zoom;p.setStyle(Paint.Style.FILL);p.setColor(i==selected?Color.rgb(70,145,255):Color.rgb(87,101,123));c.drawRect(n.x-size,n.y-size,n.x+size,n.y+size,p);p.setColor(Color.WHITE);p.setTextSize(16);p.setTextAlign(Paint.Align.CENTER);c.drawText(n.name,n.x,n.y+5,p);if(n.locked){p.setStyle(Paint.Style.STROKE);p.setStrokeWidth(3);p.setColor(Color.rgb(255,200,80));c.drawRect(n.x-size-5,n.y-size-5,n.x+size+5,n.y+size+5,p);}} }
    @Override public boolean onTouchEvent(MotionEvent e){float x=e.getX(),y=e.getY();if(e.getActionMasked()==MotionEvent.ACTION_DOWN){selected=hit(x,y);dragging=selected>=0&& !nodes.get(selected).locked;sx=x;sy=y;invalidate();if(listener!=null)listener.run();return true;}if(e.getActionMasked()==MotionEvent.ACTION_MOVE&&dragging){Node n=nodes.get(selected);n.x+=x-sx;n.y+=y-sy;sx=x;sy=y;invalidate();if(listener!=null)listener.run();return true;}if(e.getActionMasked()==MotionEvent.ACTION_UP){dragging=false;return true;}return true;}
    int hit(float x,float y){for(int i=nodes.size()-1;i>=0;i--){Node n=nodes.get(i);if(Math.abs(x-n.x)<=52*zoom&&Math.abs(y-n.y)<=52*zoom)return i;}return -1;}
  }
  class Node{int id;String name;float x,y;boolean visible=true,locked=false;String texture="";int layer=0;Node(int i,String n,float X,float Y){id=i;name=n;x=X;y=Y;}Node(Node o){id=o.id;name=o.name;x=o.x;y=o.y;visible=o.visible;locked=o.locked;texture=o.texture;layer=o.layer;}}
}
