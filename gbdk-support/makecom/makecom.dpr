{$APPTYPE CONSOLE}
{$IFDEF FPC}
  {$MODE DELPHI}
{$ENDIF}
uses classes, sysutils, math;

procedure ERROR(const msg: ansistring; const params: array of const);
begin
  writeln(format(msg, params));
  writeln('USAGE: makecom <image.bin> [<image.noi>] <output.com>');
  halt(1);
end;

procedure DecodeCommaText(const Value: ansistring; result: tStringList; adelimiter: ansichar);
var P, P1 : PAnsiChar;
    S     : ansistring;
begin
  if assigned(result) then begin
    result.BeginUpdate;
    try
      result.Clear;
      P := PChar(Value);
      while P^ in [#1..#31] do inc(P);
      while P^ <> #0 do
      begin
        if P^ = '"' then
          S := AnsiExtractQuotedStr(P, '"')
        else
        begin
          P1 := P;
          while (P^ >= ' ') and (P^ <> adelimiter) do inc(P);
          SetString(S, P1, P - P1);
        end;
        result.Add(S);
        while P^ in [#1..#31] do inc(P);
        if P^ = adelimiter then
          repeat
            inc(P);
          until not (P^ in [#1..#31]);
      end;
    finally
      result.EndUpdate;
    end;
  end;
end;

function load_symbols(const filename: ansistring; symbols: tStringList): boolean;
var s   : textfile;
    str : ansistring;
    row : tStringList;
begin
  result:= assigned(symbols);
  if result then begin
    assignfile(s, filename); reset(s);
    try
      row := tStringList.create;
      try
        while not eof(s) do begin
          readln(s, str);
          DecodeCommaText(str, row, ' ');
          if (row[0] = 'DEF') then begin
            symbols.Values[row[1]] := row[2];
          end;
        end;
        result:= true;
      finally freeandnil(row); end;
    finally closefile(s); end;
  end;
end;

function CopyData(banks: tList; bank: longint; image: tMemoryStream; source_ofs, dest_ofs: longint; len: longint): boolean;
var i    : longint;
    data : tMemoryStream;
begin
  writeln('writing to bank: ', bank);
  result:= (bank < 255);
  if result then begin
    if (banks.count <= bank) then
      for i:= banks.count to bank do begin banks.Add(tMemoryStream.Create()); writeln('creating bank'); end;
    data:= banks[bank];
    data.Seek(dest_ofs, soFromBeginning);
    data.Write(pAnsiChar(image.Memory)[source_ofs], len);
  end;
end;
procedure WriteData(banks: tList; const destname: ansistring);
var i   : longint;
    ovr : ansistring;
begin
  if (banks.count > 0) then begin
    with tMemoryStream(banks[0]) do try
      writeln(format('writing program: %s', [destname]));
      SaveToFile(destname);
    finally free; end;
    for i:= 1 to banks.count - 1 do
      with tMemoryStream(banks[i]) do try
        ovr:= ChangeFileExt(destname, format('.%.3d', [i]));
        writeln(format('writing overlay: %s', [ovr]));
        SaveToFile(ovr);
      finally free; end;
  end;
end;

const section_names = '_CODE,_HOME,_BASE,_CODE_0,_INITIALIZER,_LIT,_GSINIT,_GSFINAL';
var   name_bin   : ansistring;
      name_noi   : ansistring;
      name_out   : ansistring;
      symbols    : tStringList;
      known      : tStringList;
      banks      : tList;
      source     : tMemoryStream;
      i, bank    : longint;
      name, v, l : ansistring;
      addr, len  : longint;
begin
  if (paramcount() = 2) then begin
    name_bin:= paramstr(1); name_noi:= changefileext(name_bin, '.noi'); name_out:= paramstr(2);
  end else begin
    if (paramcount() < 3) then ERROR('ERROR: Not sufficient parameters', []);
    name_bin:= paramstr(1); name_noi:= paramstr(2); name_out:= paramstr(3);
  end;

  if not fileexists(name_noi) then ERROR('ERROR: symbol file: "%s" not found', [name_noi]);
  if not fileexists(name_bin) then ERROR('ERROR: binary image: "%s" not found', [name_bin]);

  known:= tStringList.create;
  symbols := tStringList.create;
  try
    DecodeCommaText(section_names, known, ',');
    if load_symbols(name_noi, symbols) then begin
      source:= tMemoryStream.Create;
      try
        source.LoadFromFile(name_bin);
        banks:= tList.Create;
        with banks do try
          for i:= 0 to symbols.count - 1 do begin
            name:= symbols.Names[i];
            v:= symbols.Values[name];
            if (copy(name, 1, 2) = 's_') then begin
              name:= copy(name, 3, length(name));
              l:= symbols.Values[format('l_%s',[name])];
              if length(l) > 0 then begin
                if (copy(v, 1, 2)) = '0x' then begin v[1]:= ' '; v[2]:= '$'; end;
                addr:= StrToIntDef(v, 0);
                if (copy(l, 1, 2)) = '0x' then begin l[1]:= ' '; l[2]:= '$'; end;
                len:= StrToIntDef(l, 0);

                if (len > 0) then begin
                  if (known.IndexOf(name) >= 0) then begin
                    CopyData(banks, 0, source, addr, math.max(0, addr - $100), len);
                  end;
                  if (copy(Name, 1, 6) = '_CODE_') then begin
                    bank:= addr shr 16;
                    CopyData(banks, bank, source, math.max(0, (addr and $ffff) - $4000) + $4000 * bank, 0, len);
                  end;
                end;
              end;
            end;
          end;
          writeln('writing...');
          WriteData(banks, name_out);
          writeln('done!');
        finally banks.free; end;
      finally freeandnil(source); end;
    end;
  finally
    freeandnil(symbols);
    freeandnil(known);
  end;

end.