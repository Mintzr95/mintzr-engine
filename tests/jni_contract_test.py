#!/usr/bin/env python3
import re
from pathlib import Path

root = Path(__file__).resolve().parents[1]
java_files = [
    root / "android/app/src/main/java/com/mju/engine/MainActivity.java",
    root / "android/app/src/main/java/com/mju/engine/EditorActivity.java",
]
jni_cpp = root / "platform/android/android_entry.cpp"

native_methods = []
for path in java_files:
    text = path.read_text(encoding="utf-8")
    cls_match = re.search(r"public class (\w+)", text)
    if not cls_match:
        raise SystemExit(f"missing class declaration: {path}")
    class_name = cls_match.group(1)
    for method in re.finditer(r"(?:public|private|protected)\s+static\s+native\s+[\w\[\]]+\s+(native\w+)\s*\(", text):
        native_methods.append((class_name, method.group(1)))

cpp_text = jni_cpp.read_text(encoding="utf-8")
missing = []
for class_name, method_name in native_methods:
    symbol = f"Java_com_mju_engine_{class_name}_{method_name}"
    if symbol not in cpp_text:
        missing.append(symbol)

if missing:
    raise SystemExit("missing JNI exports:\n" + "\n".join(missing))

print(f"JNI contract OK: {len(native_methods)} native methods")
