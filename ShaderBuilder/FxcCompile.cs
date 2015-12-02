using System;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;
using System.Text.RegularExpressions;

namespace ShaderBuilder {

class FxcCompile
{
    //
    // Public members:

    public enum Profile
    {
        vs_2_0,
        ps_2_0,
        ps_2_b,
        vs_3_0,
        ps_3_0
    }

    public delegate void ErrorDelegate(FxcCompile o, string error);

    public delegate void ProgressDelegate(FxcCompile o, double relativeValue);

    public ErrorDelegate Error;

    public ProgressDelegate Progress;

    public bool Compile(string filePath, string outputPrefix, string fxcExe, Profile profile, string tempFolder)
    {
        DoProgress(0, 0);

        m_Profile = profile;

        m_Source.Clear();

        if (!ReadInputFile(filePath))
            return false;

        if (!FilterComobos())
            return false;

        try
        {
            m_Expression = new SimplePerlExpression(m_PerlSkipCode);
        }
        catch (ApplicationException e)
        {
            DoError("Error when creating SimplePerlExpression: "+e.ToString());
            return false;
        }

        if (!CompileCombos(outputPrefix,fxcExe,tempFolder))
            return false;

        return true;
    }

    //
    // Private members:

    private const int m_ProgressSections = 3;

    private struct Combo
    {
        public string Name;
        public int Min;
        public int Max;

        public Combo(string name, int min, int max)
        {
            Name = name;
            Min = min;
            Max = max;
        }

        public override bool Equals(object obj)
        {
            return Name.Equals(obj);
        }

        public override int GetHashCode()
        {
            return Name.GetHashCode();
        }
    }

    private enum Platform
    {
        PC,
        XBOX
    }

    private Profile m_Profile;

    private Platform m_Platform = Platform.PC;

    private bool m_SFM = false;

    private LinkedList<string> m_Source = new LinkedList<string>();

    private LinkedList<Combo> m_StaticCombos = new LinkedList<Combo>();

    private LinkedList<Combo> m_DynamicCombos = new LinkedList<Combo>();

    private LinkedList<Combo> m_AfxCombos = new LinkedList<Combo>();

    private string m_PerlSkipCode;

    SimplePerlExpression m_Expression;

    decimal m_NumCombos;
    decimal m_NumAfxCombos;
    decimal m_NumDynamicCombos;

    private void DoError(string description)
    {
        if (null != Error) Error(this, description);
    }

    private void DoProgress(int section, double subProgress)
    {
        if (null == Progress)
            return;

        double value = section / (double)m_ProgressSections + subProgress / m_ProgressSections;
        Progress(this, value);
    }

    private void CalcNumCombos()
    {
        m_NumCombos = 1;
        m_NumAfxCombos = 1;
        m_NumDynamicCombos = 1;

        foreach(Combo combo in m_AfxCombos)
        {
            decimal d = combo.Max - combo.Min + 1;
            m_NumCombos *= d;
            m_NumAfxCombos *= d;
        }
        
        foreach (Combo combo in m_DynamicCombos)
        {
            decimal d = combo.Max - combo.Min + 1;
            m_NumCombos *= d;
            m_NumDynamicCombos *= d;
        }

        foreach (Combo combo in m_StaticCombos)
        {
            decimal d = combo.Max - combo.Min + 1;
            m_NumCombos *= d;
        }
    }

    private void DefineCombos(decimal num, out string fxcDefines, out string shaderIdent)
    {
        decimal num2 = num;

        fxcDefines = "";

        foreach (Combo combo in m_AfxCombos)
        {
            decimal d = num % (combo.Max - combo.Min + 1) + combo.Min;
            m_Expression.DefineVariable(combo.Name, (int)d);
            fxcDefines += "/D" + combo.Name + "=" + d + " ";
            num = num / (combo.Max - combo.Min + 1);
            num = decimal.Floor(num);
        }

        foreach (Combo combo in m_DynamicCombos)
        {
            decimal d = num % (combo.Max - combo.Min + 1) + combo.Min;
            m_Expression.DefineVariable(combo.Name, (int)d);
            fxcDefines += "/D" + combo.Name + "=" + d + " ";
            num = num / (combo.Max - combo.Min + 1);
            num = decimal.Floor(num);
        }

        foreach (Combo combo in m_StaticCombos)
        {
            decimal d = num % (combo.Max - combo.Min + 1) + combo.Min;
            m_Expression.DefineVariable(combo.Name, (int)d);
            fxcDefines += "/D" + combo.Name + "=" + d + " ";
            num = num / (combo.Max - combo.Min + 1);
            num = decimal.Floor(num);
        }

        decimal afxCombo = num2 % m_NumAfxCombos;
        num2 /= m_NumAfxCombos;
        num2 = decimal.Floor(num2);
        decimal dynamicCombo = num2 % m_NumDynamicCombos;
        num2 /= m_NumDynamicCombos;
        num2 = decimal.Floor(num2);
        decimal staticCombo = num2;

        shaderIdent = "" + staticCombo + "_" + dynamicCombo + "_" + afxCombo + "";

        fxcDefines += "/DSHADER_MODEL_" + m_Profile.ToString().ToUpper() + "=1 ";
    }

    private bool WriteSourceFile(string tempFolder, out string sourceFilePath)
    {
        sourceFilePath = tempFolder+"\\afxTempShaderSource.fxc";
        System.IO.StreamWriter sw = null;

        try
        {
            sw = new System.IO.StreamWriter(sourceFilePath);

            foreach(string line in m_Source)
            {
                sw.WriteLine(line);
            }

            sw.Close();
        }
        catch(System.IO.IOException e)
        {
            DoError("Error writing temp file \"" + sourceFilePath + "\": " + e);
            if (null != sw) sw.Close();

            return false;
        }

        return true;
    }

    private static string EscapeCmdArgument(string arg)
    {
        return "\"" + arg.Replace ("\\", "\\\\").Replace("\"", "\\\"") + "\"";

    }

    private bool CompileCombos(string outputPrefix, string fxcExe, string tempFolder)
    {
        DoProgress(2, 0);

        CalcNumCombos();

        if (m_NumCombos > 0x7fffffff)
        {
            DoError("Too many combos.");
            return false;
        }

        string sourceFilePath;

        if(!WriteSourceFile(tempFolder, out sourceFilePath))
            return false;

        for (decimal i = 0; i < m_NumCombos; ++i)
        {
            DoProgress(2, (double)i / (double)m_NumCombos);

            string fxcDefines;
            string shaderIdent;

            DefineCombos(i, out fxcDefines, out shaderIdent);

            bool skip = m_Expression.Eval();

            if (!skip)
            {
                string outFile = outputPrefix + "_" + shaderIdent + ".fxo";
                string args = "/T " + EscapeCmdArgument(m_Profile.ToString()) + " /Fo " + EscapeCmdArgument(outFile) + " " + fxcDefines + " " + EscapeCmdArgument(sourceFilePath);

                System.Diagnostics.ProcessStartInfo psi = new System.Diagnostics.ProcessStartInfo(fxcExe, args);

                try
                {
                    System.Diagnostics.Process pr = System.Diagnostics.Process.Start(psi);
                    pr.WaitForExit();
                    if (0 != pr.ExitCode)
                    {
                        DoError(EscapeCmdArgument(psi.FileName) + " " + psi.Arguments + " failed with ExitCode: " + pr.ExitCode);
                        return false;
                    }
                }
                catch (System.IO.IOException e)
                {
                    DoError(EscapeCmdArgument(psi.FileName) + " " + psi.Arguments + " failed with IOException: " + e.ToString());
                    return false;
                }
                catch (System.ComponentModel.Win32Exception e)
                {
                    DoError(EscapeCmdArgument(psi.FileName) + " " + psi.Arguments + " failed with Win32Exception: " + e.ToString());
                    return false;
                }

            }
        }

        DoProgress(2, 1);

        return true;
    }

    private bool FilterComobos()
    {
        DoProgress(1, 0);

        m_StaticCombos.Clear();
        m_DynamicCombos.Clear();
        m_AfxCombos.Clear();
        m_PerlSkipCode = "";

        Regex regExPc = new Regex(
            @"\[PC\]"
        );

        Regex regExXbox = new Regex(
            @"\[XBOX\]"
        );

        Regex regExSfm = new Regex(
            @"\[SFM\]"
        );

        Regex regExNotSfm = new Regex(
            @"\[!SFM\]"
        );

        Regex regExPs = new Regex(
            @"(?i)\[(ps\d+\w?)\]"
        );

        Regex regExVs = new Regex(
            @"(?i)\[(vs\d+\w?)\]"
        );

        Regex regExInitExpr = new Regex(
            @"\[\s*\=\s*([^\]]+)\]"
        );

        Regex regExBracketed = new Regex(
            @"\[[^\[\]]*\]"
        );

        Regex regExEmptyLine = new Regex(
            @"^\s*$"
        );

        Regex regExStaticCombo = new Regex(
            @"^\s*\/\/\s*STATIC\s*\:\s*" +"\""+ @"(.*)" +"\""+ @"\s+"+ "\""+ @"(\d+)\.\.(\d+)" +"\""
        );

        Regex regExDynamicCombo = new Regex(
            @"^\s*\/\/\s*DYNAMIC\s*\:\s*" + "\"" + @"(.*)" + "\"" + @"\s+" + "\"" + @"(\d+)\.\.(\d+)" + "\""
        );

        Regex regExAfxCombo = new Regex(
            @"^\s*\/\/\s*AFX\s*\:\s*" + "\"" + @"(.*)" + "\"" + @"\s+" + "\"" + @"(\d+)\.\.(\d+)" + "\""
        );

        Regex regExSkipCode = new Regex(
            @"^\s*\/\/\s*SKIP\s*\s*\:\s*(.*)$"
        );
        
        string profileStringLower = m_Profile.ToString().Replace("_","").ToLower();

        int lineNum = 0;
        double orgCount = m_Source.Count;

        for (LinkedListNode<string> node = m_Source.First; null != node; )
        {
            string orginalLine = node.Value;

            DoProgress(1, lineNum / orgCount);

            try
            {
                string value = node.Value;

                Match matchPc = regExPc.Match(value);
                Match matchXbox = regExXbox.Match(value);
                Match matchSfm = regExSfm.Match(value);
                Match matchNotSfm = regExNotSfm.Match(value);
                Match matchPs = regExPs.Match(value);
                Match matchVs = regExVs.Match(value);

                if (
                    matchPc.Success && m_Platform != Platform.PC
                    || matchXbox.Success && m_Platform != Platform.XBOX
                    || matchSfm.Success && !m_SFM
                    || matchNotSfm.Success && m_SFM
                    || matchPs.Success && 0 != matchPs.Groups[1].Value.ToLower().CompareTo(profileStringLower)
                    || matchVs.Success && 0 != matchVs.Groups[1].Value.ToLower().CompareTo(profileStringLower)
                )
                {
                    LinkedListNode<string> next = node.Next;
                    m_Source.Remove(node);
                    node = next;
                    ++lineNum;
                    continue;
                }

                string initExpression = null;
                Match matchInitExpr = regExInitExpr.Match(value);
                if (matchInitExpr.Success) initExpression = matchInitExpr.Groups[1].Value;

                value = regExBracketed.Replace(value, "");

                Match matchEmptyLine = regExEmptyLine.Match(value);
                if (matchEmptyLine.Success)
                {
                    node = node.Next;
                    ++lineNum;
                    continue;
                }

                Match matchStaticCombo = regExStaticCombo.Match(value);
                if (matchStaticCombo.Success)
                {
                    string name = matchStaticCombo.Groups[1].Value;
                    string min = matchStaticCombo.Groups[2].Value;
                    string max = matchStaticCombo.Groups[3].Value;

                    Combo combo = new Combo(name, int.Parse(min), int.Parse(max));

                    if (m_StaticCombos.Contains(combo))
                        throw new ArgumentException("m_StaticCombos already contains a combo with name " + name + ".");
                    m_StaticCombos.AddLast(combo);
                }

                Match matchDynamicCombo = regExDynamicCombo.Match(value);
                if (matchDynamicCombo.Success)
                {
                    string name = matchDynamicCombo.Groups[1].Value;
                    string min = matchDynamicCombo.Groups[2].Value;
                    string max = matchDynamicCombo.Groups[3].Value;

                    Combo combo = new Combo(name, int.Parse(min), int.Parse(max));

                    if (m_DynamicCombos.Contains(combo))
                        throw new ArgumentException("m_DynamicCombos already contains a combo with name " + name + ".");
                    m_DynamicCombos.AddLast(combo);
                }

                Match matchAfxCombo = regExAfxCombo.Match(value);
                if (matchAfxCombo.Success)
                {
                    string name = matchAfxCombo.Groups[1].Value;
                    string min = matchAfxCombo.Groups[2].Value;
                    string max = matchAfxCombo.Groups[3].Value;

                    Combo combo = new Combo(name, int.Parse(min), int.Parse(max));

                    if (m_AfxCombos.Contains(combo))
                        throw new ArgumentException("m_AfxCombos already contains a combo with name " + name + ".");
                    m_AfxCombos.AddLast(combo);
                }

                Match matchSkipCode = regExSkipCode.Match(value);
                if (matchSkipCode.Success)
                {
                    m_PerlSkipCode += "(" + matchSkipCode.Groups[1].Value + ")||";
                }
            }
            catch(ApplicationException e)
            {
                DoError("FilterComobos: Error in: Line #" + lineNum + " (" + orginalLine + "): " + e);
                return false;
            }

            node = node.Next;
            ++lineNum;
        }

        m_PerlSkipCode = m_PerlSkipCode + "0";

        DoProgress(1, 1);

        return true;
    }

    private bool ReadInputFile(String filePath)
    {
        System.IO.StreamReader sr = null;

        Regex includeRegEx = new Regex(
            @"(?i)\#include\s" + "\"" + @"(.*)" + "\""
        );

        try
        {
            sr = new System.IO.StreamReader(filePath);

            string line;

            int lineNum = 0;

            while(null != (line = sr.ReadLine()))
            {
                Match match = includeRegEx.Match(line);

                if (match.Success)
                {
                    string basePath = System.IO.Path.GetDirectoryName(filePath);
                    string subFilePath = basePath + "\\" + match.Groups[1].Value;

                    bool subFileOk = ReadInputFile(subFilePath);

                    if (!subFileOk)
                    {
                        DoError("Error doing include in \"" + filePath + "\" in line " + lineNum + ".");

                        sr.Close();
                        return false;
                    }

                    match.NextMatch();
                }
                else
                    m_Source.AddLast(line);

                ++lineNum;
            }

            sr.Close();
        }
        catch(System.IO.IOException e)
        {
            DoError("Error reading file \"" + filePath + "\": " + e);
            if (null != sr) sr.Close();

            return false;
        }

        return true;
    }

}

} // namespace ShaderBuilder {
