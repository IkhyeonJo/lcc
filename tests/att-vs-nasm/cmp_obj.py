"""AT&T (x86/linux) vs NASM (x86/nasm) equivalence check.
Code : objdump -d (intel) of the .o files; padding dropped; branch targets ->
       instruction index; relocated displacement values masked, relocation type
       + target class (external symbol name / LOCAL) compared.
Data : data directives in the .s files normalized to (kind, value) sequence.
usage: python3 -I cmp_obj.py <out-dir> [known-diff ...]
  known-diff: "<suite>/<test>" whose code is allowed to differ (documented
  instruction-selection differences); anything else that differs -> exit 1.
"""
import difflib, glob, os, re, subprocess, sys

def sh(*a):
    return subprocess.run(a, capture_output=True, text=True).stdout

INS = re.compile(r"^\s*([0-9a-f]+):\s+(.*)$")
RELOC = re.compile(r"^\s*([0-9a-f]+): (R_386_\w+)\s+(\S+)")
PAD = re.compile(r"^(nop\b|xchg ax,ax|lea esi,\[esi(\+eiz\*1)?\+0x0\]|data16|cs nop|lea edi,\[edi(\+eiz\*1)?\+0x0\]|lea esi,\[esi\+0x0\]|lea (esi|edi),(DWORD PTR )?\[(esi|edi)(\+eiz\*1)?\+0x0\])")

def externs(o):
    g = set()
    for l in sh("nm", o).splitlines():
        p = l.split()
        if len(p) >= 2 and p[-2] in ("U",):
            g.add(p[-1])
    return g

def funcs(o):
    f = {}
    for l in sh("nm", o).splitlines():
        p = l.split()
        if len(p) == 3 and p[1] in "Tt" and not p[2].startswith(("L", ".L")):
            f[p[2]] = int(p[0], 16)
    return f

def code(o):
    ext = externs(o); fn = funcs(o)
    rows = []
    for line in sh("objdump", "-d", "-r", "-M", "intel", "--no-show-raw-insn", "-j", ".text", o).splitlines():
        m = RELOC.match(line)
        if m and rows:
            sym = re.sub(r"[-+]0x[0-9a-f]+$", "", m.group(3))
            if m.group(2) == "R_386_PC32" and sym in fn and rows[-1][1].startswith("call"):
                rows[-1][1] = "call FN:" + sym     # GAS: reloc against own global function
                continue
            rows[-1][2].append(m.group(2) + ":" + (sym if sym in ext else "LOCAL"))
            continue
        m = INS.match(line)
        if m:
            rows.append([int(m.group(1), 16), re.sub(r"\s+", " ", m.group(2).strip()), []])
    rows = [r for r in rows if r[2] or not PAD.match(r[1])]   # never drop an insn that carries a relocation
    idx = {r[0]: i for i, r in enumerate(rows)}
    byaddr = {a: n for n, a in fn.items()}
    for r in rows:   # NASM: direct call to own function
        m = re.match(r"call ([0-9a-f]+) <([^>+]+)>$", r[1])
        if m and int(m.group(1), 16) in byaddr:
            r[1] = "call FN:" + byaddr[int(m.group(1), 16)]
    out = []
    for a, t, rel in rows:
        t = re.sub(r"\b([0-9a-f]+) <[^>]+>", lambda m: "@%s" % idx.get(int(m.group(1), 16), "?"), t)
        t = re.sub(r"\b(DWORD|WORD|BYTE|QWORD|TBYTE) PTR ", r"\1 ", t)
        t = t.replace("ds:", "")
        # index*2 with no base: NASM encodes [r*2] as [r+r*1] (same address, shorter)
        # (keep the displacement: with a relocation it is the symbol, not a real 0)
        t = re.sub(r"\[(\w+)\*2([-+]0x[0-9a-f]+)\]", r"[\1+\1*1\2]", t)
        if not rel:
            t = t.replace("*1+0x0]", "*1]")
        if rel:  # displacement/immediate comes from the relocation; compare symbolically
            t = re.sub(r"[-+]?0x[0-9a-f]+", "+R", t).replace(" +R", " R").replace(",+R", ",R").replace("[+R", "[R")
        out.append("%s  %s" % (t, ",".join(rel)))
    return out

def unesc_att(s):
    # GAS .ascii "..." with C escapes / octal
    b = bytearray(); i = 0
    while i < len(s):
        c = s[i]
        if c == "\\":
            i += 1; c = s[i]
            if c in "01234567":
                j = i
                while j < len(s) and j < i + 3 and s[j] in "01234567": j += 1
                b.append(int(s[i:j], 8)); i = j; continue
            b.append({"n": 10, "t": 9, "\\": 92, '"': 34, "r": 13, "b": 8, "f": 12}.get(c, ord(c)))
        else:
            b.append(ord(c))
        i += 1
    return bytes(b)

def num(v):
    v = v.strip()
    v = re.sub(r"^\$?\.?L(S?)(\d+)", r"L\2", v)
    v = re.sub(r"^\$", "", v)
    if re.fullmatch(r"-?[0-9][0-9a-fA-F]*[hH]", v):
        v = ("-0x" + v[1:-1]) if v.startswith("-") else ("0x" + v[:-1])
    try:
        return str(int(v, 0) & 0xffffffff)
    except ValueError:
        return v

def data_att(path):
    seq = []
    for l in open(path, errors="replace"):
        l = l.strip()
        m = re.match(r"\.(byte|short|long)\s+(.*)$", l)
        if m:
            k = {"byte": 1, "short": 2, "long": 4}[m.group(1)]
            for v in m.group(2).split(","):
                seq.append(("%d" % k, num(v)))
            continue
        m = re.match(r'\.ascii\s+"(.*)"$', l)
        if m:
            for ch in unesc_att(m.group(1)): seq.append(("1", str(ch)))
            continue
        m = re.match(r"\.(space|skip)\s+(\d+)", l)
        if m: seq.append(("space", m.group(2))); continue
        m = re.match(r"\.(comm|lcomm)\s+(\S+),\s*(\d+)", l)
        if m: seq.append(("space", m.group(3))); continue
    return merge(seq)

def data_nasm(path):
    seq = []
    for l in open(path, errors="replace"):
        l = l.strip()
        m = re.match(r"(db|dw|dd)\s+(.*)$", l)
        if m:
            k = {"db": 1, "dw": 2, "dd": 4}[m.group(1)]
            for v in split_nasm(m.group(2)):
                if v.startswith(("'", '"')):
                    for ch in v[1:-1].encode("latin-1"): seq.append(("1", str(ch)))
                else:
                    seq.append(("%d" % k, num(v)))
            continue
        m = re.match(r"resb\s+(\d+)", l)
        if m: seq.append(("space", m.group(1))); continue
        m = re.match(r"times\s+(\d+)\s+db\s+0$", l)
        if m: seq.append(("space", m.group(1))); continue
        m = re.match(r"common\s+\S+\s+(\d+)", l)
        if m: seq.append(("space", m.group(1))); continue
    return merge(seq)

def split_nasm(s):
    out, cur, q = [], "", None
    for c in s:
        if q:
            cur += c
            if c == q: q = None
        elif c in "'\"":
            q = c; cur += c
        elif c == ",":
            out.append(cur.strip()); cur = ""
        else:
            cur += c
    if cur.strip(): out.append(cur.strip())
    return out

def merge(seq):
    # zero-filled .space and db 0 are the same bytes
    out = []
    for k, v in seq:
        if k == "space":
            out.extend([("1", "0")] * int(v))
        else:
            out.append((k, v))
    return out

def main(outdir, known):
    total = ok = 0
    unexpected = []
    for att_o in sorted(glob.glob(os.path.join(outdir, "*", "att", "*.o"))):
        nasm_o = att_o.replace("/att/", "/nasm/")
        if not os.path.exists(nasm_o): continue
        total += 1
        ca, cn = code(att_o), code(nasm_o)
        da, dn = data_att(att_o[:-2] + ".s"), data_nasm(nasm_o[:-2] + ".s")
        if ca == cn and da == dn:
            ok += 1; continue
        name = os.path.relpath(att_o, outdir)
        key = name.split("/")[0] + "/" + os.path.basename(name)[:-2]
        if key in known:
            print("KNOWN-DIFF %s (documented instruction-selection difference)" % key)
            continue
        unexpected.append(key)
        print("DIFF %s%s%s  (insns att=%d nasm=%d, data att=%d nasm=%d)" % (name, " CODE" if ca != cn else "", " DATA" if da != dn else "", len(ca), len(cn), len(da), len(dn)))
        if ca != cn:
            for l in list(difflib.unified_diff(ca, cn, "att", "nasm", n=2, lineterm=""))[2:14]: print("    " + l)
        if da != dn:
            for l in list(difflib.unified_diff([str(x) for x in da], [str(x) for x in dn], "att", "nasm", n=1, lineterm=""))[2:10]: print("    " + l)
    print("\nobjects compared: %d  equivalent: %d  known-diff: %d  unexpected-diff: %d"
          % (total, ok, total - ok - len(unexpected), len(unexpected)))
    return 1 if unexpected else 0

sys.exit(main(sys.argv[1], set(sys.argv[2:])))
