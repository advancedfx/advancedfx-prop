object Form1: TForm1
  Left = 227
  Top = 274
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'HL Demo Fix 1.0.1'
  ClientHeight = 221
  ClientWidth = 360
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 176
    Width = 345
    Height = 41
    AutoSize = False
    Caption = 
      'Copyright (c) 2006 by matrixstorm.com and hhg-clan.de'#13#10'Informati' +
      'ons from Uwe Girlich'#13#10'You are using this program at your own ris' +
      'k!'
    WordWrap = True
  end
  object ProgressBar1: TProgressBar
    Left = 8
    Top = 40
    Width = 345
    Height = 17
    Min = 0
    Max = 1024
    Smooth = True
    TabOrder = 0
  end
  object Button1: TButton
    Left = 8
    Top = 8
    Width = 345
    Height = 25
    Caption = 'Fix a Demo'
    TabOrder = 1
    OnClick = Button1Click
  end
  object Memo1: TMemo
    Left = 8
    Top = 64
    Width = 345
    Height = 97
    Color = clBtnFace
    Lines.Strings = (
      
        'For more Information read the included readme.txt (liesmich.txt)' +
        '.'
      ''
      'Click on Fix a Demo to begin the process.')
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 2
  end
  object OpenDialog1: TOpenDialog
    Filter = 'Half-Life Demo File|*.dem|'
    Title = 'Selct Demo File to fix'
    Left = 16
  end
  object SaveDialog1: TSaveDialog
    DefaultExt = 'dem'
    Filter = 'Half-Life Demo File|*.dem|'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableSizing]
    Title = 'Save Fixed File To ...'
    Left = 48
  end
end
