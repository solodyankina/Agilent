object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'Form1'
  ClientHeight = 739
  ClientWidth = 751
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 456
    Top = 45
    Width = 56
    Height = 13
    Caption = #1040#1084#1087#1083#1080#1090#1091#1076#1072
  end
  object Label2: TLabel
    Left = 456
    Top = 113
    Width = 62
    Height = 13
    Caption = #1054#1090#1082#1083#1086#1085#1077#1085#1080#1077
  end
  object Label3: TLabel
    Left = 592
    Top = 45
    Width = 107
    Height = 13
    Caption = #1053#1072#1088#1072#1089#1090#1072#1085#1080#1077' '#1092#1088#1086#1085#1072#1090#1072
  end
  object Label4: TLabel
    Left = 616
    Top = 113
    Width = 67
    Height = 13
    Caption = #1057#1087#1072#1076' '#1092#1088#1086#1085#1090#1072
  end
  object Label5: TLabel
    Left = 287
    Top = 113
    Width = 22
    Height = 13
    Caption = 'Time'
  end
  object RichEdit1: TRichEdit
    Left = 8
    Top = 8
    Width = 273
    Height = 145
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    ScrollBars = ssVertical
    TabOrder = 0
  end
  object Button1: TButton
    Left = 287
    Top = 40
    Width = 75
    Height = 25
    Caption = #1040#1074#1090#1086#1079#1072#1084#1077#1088#1099
    TabOrder = 1
    OnClick = Button1Click
  end
  object Chart1: TChart
    Left = 32
    Top = 159
    Width = 681
    Height = 378
    AllowPanning = pmVertical
    Legend.Visible = False
    Title.Text.Strings = (
      'TChart')
    DepthAxis.Automatic = False
    DepthAxis.AutomaticMaximum = False
    DepthAxis.AutomaticMinimum = False
    DepthAxis.Maximum = -0.159999999999884100
    DepthAxis.Minimum = -1.159999999999877000
    DepthTopAxis.Automatic = False
    DepthTopAxis.AutomaticMaximum = False
    DepthTopAxis.AutomaticMinimum = False
    DepthTopAxis.Maximum = -0.159999999999884100
    DepthTopAxis.Minimum = -1.159999999999877000
    RightAxis.Automatic = False
    RightAxis.AutomaticMaximum = False
    RightAxis.AutomaticMinimum = False
    View3D = False
    TabOrder = 2
    PrintMargins = (
      15
      19
      15
      19)
    object Series1: TLineSeries
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = False
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
  end
  object Edit1: TEdit
    Left = 424
    Top = 64
    Width = 121
    Height = 21
    TabOrder = 3
  end
  object Edit2: TEdit
    Left = 424
    Top = 132
    Width = 121
    Height = 21
    TabOrder = 4
  end
  object Edit3: TEdit
    Left = 592
    Top = 64
    Width = 121
    Height = 21
    TabOrder = 5
  end
  object Edit4: TEdit
    Left = 592
    Top = 132
    Width = 121
    Height = 21
    TabOrder = 6
  end
  object Button2: TButton
    Left = 640
    Top = 543
    Width = 33
    Height = 25
    Caption = '>'
    TabOrder = 7
    OnClick = Button2Click
  end
  object Button3: TButton
    Left = 668
    Top = 543
    Width = 45
    Height = 25
    Caption = '>>'
    TabOrder = 8
    OnClick = Button3Click
  end
  object Edit5: TEdit
    Left = 287
    Top = 132
    Width = 91
    Height = 21
    TabOrder = 9
  end
  object ComboBox1: TComboBox
    Left = 287
    Top = 8
    Width = 154
    Height = 21
    TabOrder = 10
    Text = #1042#1099#1073#1077#1088#1077#1090#1077' '#1088#1077#1078#1080#1084' '#1079#1072#1087#1091#1089#1082#1072
    Items.Strings = (
      'DSTArt '
      'DSTOp'
      'CSTArt'
      'CSTOp'
      'RTA'
      'PERRor'
      'SERRor'
      'MERRor'
      'RTA11')
  end
  object Button4: TButton
    Left = 264
    Top = 558
    Width = 98
    Height = 25
    Caption = #1057#1086#1079#1076#1072#1090#1100' '#1084#1072#1089#1082#1091
    TabOrder = 11
    OnClick = Button4Click
  end
  object RichEdit2: TRichEdit
    Left = 32
    Top = 560
    Width = 226
    Height = 163
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    ScrollBars = ssVertical
    TabOrder = 12
  end
  object Button5: TButton
    Left = 264
    Top = 698
    Width = 98
    Height = 25
    Caption = #1054#1095#1080#1089#1090#1080#1090#1100' '#1084#1072#1089#1082#1091
    TabOrder = 13
    OnClick = Button5Click
  end
  object Button8: TButton
    Left = 264
    Top = 600
    Width = 75
    Height = 25
    Caption = #1057#1090#1072#1088#1090
    TabOrder = 14
    OnClick = Button8Click
  end
  object SaveDialog1: TSaveDialog
    Left = 688
    Top = 688
  end
end
