package com.mju.engine;

import android.app.Activity;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.drawable.GradientDrawable;
import android.os.Bundle;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;
import java.io.File;
import java.util.ArrayList;

public class EditorActivity extends Activity {
    private EditorCanvas canvas;
    private TextView inspectorText;
    private LinearLayout hierarchy;
    private EditText nameEdit;
    private EditText xEdit;
    private EditText yEdit;
    private CheckBox visibleBox;
    private CheckBox lockedBox;
    private SeekBar zoom;
    private int selectedId = 0;
    private final ArrayList<NodeView> nodes = new ArrayList<>();

    @Override
    protected void onCreate(Bundle state) {
        super.onCreate(state);
        nativeEditorStart();
        buildUi();
        refreshAll();
    }

    private TextView label(String text) {
        TextView view = new TextView(this);
        view.setText(text);
        view.setTextColor(Color.WHITE);
        view.setGravity(Gravity.CENTER_VERTICAL | Gravity.RIGHT);
        view.setPadding(12, 0, 12, 0);
        return view;
    }

    private Button action(String text) {
        Button button = new Button(this);
        button.setText(text);
        button.setTextColor(Color.WHITE);
        button.setAllCaps(false);
        button.setMinHeight(0);
        button.setPadding(12, 0, 12, 0);
        GradientDrawable background = new GradientDrawable();
        background.setColor(Color.rgb(35, 45, 60));
        background.setCornerRadius(18);
        button.setBackground(background);
        return button;
    }

    private EditText field(String hint) {
        EditText edit = new EditText(this);
        edit.setHint(hint);
        edit.setTextColor(Color.WHITE);
        edit.setHintTextColor(Color.rgb(150, 160, 175));
        edit.setSingleLine(true);
        edit.setPadding(12, 0, 12, 0);
        return edit;
    }

    private void buildUi() {
        LinearLayout root = new LinearLayout(this);
        root.setOrientation(LinearLayout.VERTICAL);
        root.setBackgroundColor(Color.rgb(13, 17, 24));

        LinearLayout bar = new LinearLayout(this);
        bar.setGravity(Gravity.CENTER_VERTICAL);
        bar.setPadding(8, 8, 8, 8);
        TextView title = label("MJU • محرر اللعبة");
        title.setTextSize(18);
        bar.addView(title, new LinearLayout.LayoutParams(0, 56, 1));

        Button add = action("+ عنصر");
        Button dup = action("نسخ");
        Button del = action("حذف");
        Button undo = action("تراجع");
        Button redo = action("إعادة");
        Button save = action("حفظ");
        Button play = action("تشغيل");
        for (Button button : new Button[]{add, dup, del, undo, redo, save, play}) {
            bar.addView(button, new LinearLayout.LayoutParams(-2, 56));
        }
        root.addView(bar);

        LinearLayout body = new LinearLayout(this);
        body.setOrientation(LinearLayout.HORIZONTAL);

        LinearLayout tree = new LinearLayout(this);
        tree.setOrientation(LinearLayout.VERTICAL);
        tree.setBackgroundColor(Color.rgb(21, 26, 35));
        TextView treeHeader = label("المشهد / Hierarchy");
        treeHeader.setTextSize(15);
        tree.addView(treeHeader, new LinearLayout.LayoutParams(220, 54));
        ScrollView scroll = new ScrollView(this);
        hierarchy = new LinearLayout(this);
        hierarchy.setOrientation(LinearLayout.VERTICAL);
        scroll.addView(hierarchy);
        tree.addView(scroll, new LinearLayout.LayoutParams(220, 0, 1));
        body.addView(tree, new LinearLayout.LayoutParams(220, -1));

        FrameLayout canvasHolder = new FrameLayout(this);
        canvas = new EditorCanvas();
        canvasHolder.addView(canvas, new FrameLayout.LayoutParams(-1, -1));
        LinearLayout zoomBar = new LinearLayout(this);
        zoomBar.setPadding(10, 6, 10, 6);
        zoomBar.setGravity(Gravity.CENTER_VERTICAL);
        zoomBar.addView(label("تكبير"), new LinearLayout.LayoutParams(70, 44));
        zoom = new SeekBar(this);
        zoom.setMax(375);
        zoom.setProgress(75);
        zoomBar.addView(zoom, new LinearLayout.LayoutParams(180, 44));
        FrameLayout.LayoutParams zoomParams = new FrameLayout.LayoutParams(260, 56, Gravity.TOP | Gravity.RIGHT);
        canvasHolder.addView(zoomBar, zoomParams);
        body.addView(canvasHolder, new LinearLayout.LayoutParams(0, -1, 1));

        LinearLayout inspector = new LinearLayout(this);
        inspector.setOrientation(LinearLayout.VERTICAL);
        inspector.setPadding(12, 12, 12, 12);
        inspector.setBackgroundColor(Color.rgb(23, 29, 39));
        TextView inspectorHeader = label("Inspector");
        inspectorHeader.setTextSize(16);
        inspector.addView(inspectorHeader, new LinearLayout.LayoutParams(260, 50));
        inspectorText = label("لا يوجد تحديد");
        inspectorText.setTextSize(13);
        inspector.addView(inspectorText, new LinearLayout.LayoutParams(260, 60));
        nameEdit = field("اسم العنصر");
        inspector.addView(nameEdit, new LinearLayout.LayoutParams(260, 48));
        LinearLayout xy = new LinearLayout(this);
        xEdit = field("X");
        yEdit = field("Y");
        xy.addView(xEdit, new LinearLayout.LayoutParams(0, 48, 1));
        xy.addView(yEdit, new LinearLayout.LayoutParams(0, 48, 1));
        inspector.addView(xy, new LinearLayout.LayoutParams(260, 52));
        visibleBox = new CheckBox(this);
        visibleBox.setText("مرئي");
        visibleBox.setTextColor(Color.WHITE);
        inspector.addView(visibleBox, new LinearLayout.LayoutParams(260, 46));
        lockedBox = new CheckBox(this);
        lockedBox.setText("مقفل");
        lockedBox.setTextColor(Color.WHITE);
        inspector.addView(lockedBox, new LinearLayout.LayoutParams(260, 46));
        TextView help = label("كل الكيانات محفوظة في C++ Scene. هذه الواجهة تعرضها وتحررها فقط.");
        help.setGravity(Gravity.TOP | Gravity.RIGHT);
        help.setTextSize(12);
        inspector.addView(help, new LinearLayout.LayoutParams(260, -1));
        body.addView(inspector, new LinearLayout.LayoutParams(260, -1));
        root.addView(body, new LinearLayout.LayoutParams(-1, 0, 1));
        setContentView(root);

        add.setOnClickListener(v -> {
            selectedId = nativeEditorCreate();
            refreshAll();
        });
        dup.setOnClickListener(v -> {
            if (selectedId != 0) selectedId = nativeEditorDuplicate(selectedId);
            refreshAll();
        });
        del.setOnClickListener(v -> {
            if (selectedId != 0 && nativeEditorDelete(selectedId)) selectedId = 0;
            refreshAll();
        });
        undo.setOnClickListener(v -> {
            if (nativeEditorUndo()) selectedId = 0;
            refreshAll();
        });
        redo.setOnClickListener(v -> {
            if (nativeEditorRedo()) selectedId = 0;
            refreshAll();
        });
        save.setOnClickListener(v -> {
            File dir = new File(getFilesDir(), "project");
            if (!dir.exists()) dir.mkdirs();
            if (nativeEditorSave(new File(dir, "main.mju").getAbsolutePath())) toast("تم حفظ المشهد");
            else toast("تعذر حفظ المشهد");
        });
        play.setOnClickListener(v -> startActivity(new android.content.Intent(this, MainActivity.class)));

        zoom.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                canvas.zoom = 0.25f + progress / 100.0f;
                canvas.invalidate();
            }
            @Override public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        View.OnFocusChangeListener focus = (view, hasFocus) -> {
            if (!hasFocus) applyInspector();
        };
        nameEdit.setOnFocusChangeListener(focus);
        xEdit.setOnFocusChangeListener(focus);
        yEdit.setOnFocusChangeListener(focus);
        visibleBox.setOnClickListener(v -> applyInspector());
        lockedBox.setOnClickListener(v -> applyInspector());
    }

    private void refreshAll() {
        refreshFromNative();
        refreshHierarchy();
        refreshInspector();
        canvas.invalidate();
    }

    private void refreshFromNative() {
        nodes.clear();
        String snapshot = nativeEditorSnapshot();
        for (String line : snapshot.split("\\n")) {
            if (line.trim().isEmpty()) continue;
            String[] fields = line.split("\\|", -1);
            if (fields.length < 7) continue;
            try {
                nodes.add(new NodeView(
                        Integer.parseInt(fields[0]), fields[1],
                        Float.parseFloat(fields[2]), Float.parseFloat(fields[3]),
                        "1".equals(fields[4]), "1".equals(fields[5]),
                        Integer.parseInt(fields[6])));
            } catch (Exception ignored) {
            }
        }
        if (selectedId != 0 && find(selectedId) == null) selectedId = 0;
    }

    private NodeView find(int id) {
        for (NodeView node : nodes) if (node.id == id) return node;
        return null;
    }

    private void refreshHierarchy() {
        hierarchy.removeAllViews();
        for (NodeView node : nodes) {
            Button button = action((node.id == selectedId ? "◆ " : "• ") + node.name);
            button.setGravity(Gravity.RIGHT);
            button.setOnClickListener(v -> {
                selectedId = node.id;
                refreshInspector();
                refreshHierarchy();
                canvas.invalidate();
            });
            hierarchy.addView(button, new LinearLayout.LayoutParams(-1, 48));
        }
    }

    private void refreshInspector() {
        NodeView node = find(selectedId);
        if (node == null) {
            inspectorText.setText("لا يوجد تحديد");
            nameEdit.setText("");
            xEdit.setText("");
            yEdit.setText("");
            visibleBox.setChecked(false);
            lockedBox.setChecked(false);
            return;
        }
        inspectorText.setText("ID: " + node.id + "\nLayer: " + node.layer);
        nameEdit.setText(node.name);
        xEdit.setText(String.valueOf(Math.round(node.x)));
        yEdit.setText(String.valueOf(Math.round(node.y)));
        visibleBox.setChecked(node.visible);
        lockedBox.setChecked(node.locked);
    }

    private void applyInspector() {
        if (selectedId == 0) return;
        try {
            nativeEditorSetName(selectedId, nameEdit.getText().toString().trim().isEmpty() ? "Node" : nameEdit.getText().toString().trim());
            float x = Float.parseFloat(xEdit.getText().toString());
            float y = Float.parseFloat(yEdit.getText().toString());
            nativeEditorSetTransform(selectedId, x, y);
        } catch (Exception ignored) {
        }
        nativeEditorSetVisibility(selectedId, visibleBox.isChecked());
        nativeEditorSetLocked(selectedId, lockedBox.isChecked());
        refreshAll();
    }

    private void toast(String text) {
        Toast.makeText(this, text, Toast.LENGTH_SHORT).show();
    }

    private final class EditorCanvas extends View {
        private final Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
        float zoom = 1.0f;
        float lastX;
        float lastY;
        boolean dragging;

        EditorCanvas() {
            super(EditorActivity.this);
            setFocusable(true);
        }

        @Override
        protected void onDraw(Canvas canvas) {
            super.onDraw(canvas);
            paint.setStyle(Paint.Style.FILL);
            paint.setColor(Color.rgb(15, 20, 28));
            canvas.drawRect(0, 0, getWidth(), getHeight(), paint);

            paint.setStrokeWidth(1);
            paint.setColor(Color.rgb(36, 44, 57));
            float step = 32 * zoom;
            for (float x = 0; x < getWidth(); x += step) canvas.drawLine(x, 0, x, getHeight(), paint);
            for (float y = 0; y < getHeight(); y += step) canvas.drawLine(0, y, getWidth(), y, paint);

            for (NodeView node : nodes) {
                if (!node.visible) continue;
                float size = 48 * zoom;
                float px = node.x * zoom;
                float py = node.y * zoom;
                paint.setStyle(Paint.Style.FILL);
                paint.setColor(node.id == selectedId ? Color.rgb(70, 145, 255) : Color.rgb(87, 101, 123));
                canvas.drawRect(px - size, py - size, px + size, py + size, paint);
                paint.setColor(Color.WHITE);
                paint.setTextSize(16);
                paint.setTextAlign(Paint.Align.CENTER);
                canvas.drawText(node.name, px, py + 5, paint);
                if (node.locked) {
                    paint.setStyle(Paint.Style.STROKE);
                    paint.setStrokeWidth(3);
                    paint.setColor(Color.rgb(255, 200, 80));
                    canvas.drawRect(px - size - 5, py - size - 5, px + size + 5, py + size + 5, paint);
                }
            }
        }

        @Override
        public boolean onTouchEvent(MotionEvent event) {
            float x = event.getX() / zoom;
            float y = event.getY() / zoom;
            if (event.getActionMasked() == MotionEvent.ACTION_DOWN) {
                selectedId = hit(x, y);
                NodeView node = find(selectedId);
                dragging = node != null && !node.locked;
                if (dragging) nativeEditorBeginTransform();
                lastX = x;
                lastY = y;
                refreshInspector();
                refreshHierarchy();
                invalidate();
                return true;
            }
            if (event.getActionMasked() == MotionEvent.ACTION_MOVE && dragging) {
                NodeView node = find(selectedId);
                if (node != null) {
                    node.x += x - lastX;
                    node.y += y - lastY;
                    nativeEditorSetTransform(node.id, node.x, node.y);
                    lastX = x;
                    lastY = y;
                    invalidate();
                    refreshInspector();
                }
                return true;
            }
            if (event.getActionMasked() == MotionEvent.ACTION_UP || event.getActionMasked() == MotionEvent.ACTION_CANCEL) {
                if (dragging) nativeEditorEndTransform();
                dragging = false;
                refreshFromNative();
                refreshHierarchy();
                refreshInspector();
                invalidate();
                return true;
            }
            return true;
        }

        private int hit(float x, float y) {
            for (int i = nodes.size() - 1; i >= 0; --i) {
                NodeView node = nodes.get(i);
                if (Math.abs(x - node.x) <= 52 && Math.abs(y - node.y) <= 52) return node.id;
            }
            return 0;
        }
    }

    private static final class NodeView {
        final int id;
        final String name;
        float x;
        float y;
        final boolean visible;
        final boolean locked;
        final int layer;

        NodeView(int id, String name, float x, float y, boolean visible, boolean locked, int layer) {
            this.id = id;
            this.name = name;
            this.x = x;
            this.y = y;
            this.visible = visible;
            this.locked = locked;
            this.layer = layer;
        }
    }

    private static native void nativeEditorStart();
    private static native String nativeEditorSnapshot();
    private static native int nativeEditorCreate();
    private static native int nativeEditorDuplicate(int id);
    private static native boolean nativeEditorDelete(int id);
    private static native boolean nativeEditorUndo();
    private static native boolean nativeEditorRedo();
    private static native void nativeEditorBeginTransform();
    private static native boolean nativeEditorSetTransform(int id, float x, float y);
    private static native void nativeEditorEndTransform();
    private static native boolean nativeEditorSetName(int id, String name);
    private static native boolean nativeEditorSetVisibility(int id, boolean visible);
    private static native boolean nativeEditorSetLocked(int id, boolean locked);
    private static native boolean nativeEditorSave(String path);
    private static native void nativeEditorClear();

    static {
        System.loadLibrary("mju_android");
    }
}
