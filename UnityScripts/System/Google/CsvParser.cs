using System.IO;
using System.Text;
using System.Collections;
using System.Collections.Generic;
 
public class CsvObject
{
    public List<List<string>> rows;
 
    public CsvObject()
    {
        rows = new();
    }
}
 
public static class CsvParser
{
    public static CsvObject ParseFile(string filePath)
    {
        try
        {
            using var reader = File.OpenText(filePath);
            string text = reader.ReadToEnd();
            return Parse(text);
        }
        catch
        {
            return null;
        }
    }
 
    public static CsvObject Parse(string text)
    {
        text += '\n'; // remove special case EOF
 
        CsvObject result = new();
        result.rows.Add(new()); // add initial row
 
        StringBuilder valueStrBuilder = new();
        bool inQuote = false;
 
        for (int i = 0; i < text.Length; ++i)
        {
            if (inQuote)
            {
                // handle quote
 
                if (i == text.Length - 1)
                {
                    // last byte
                    if (text[i] == '"')
                    {
                        // end quote
                        inQuote = false;
                        break;
                    }
                    else
                    {
                        // error: no closing quote found
                        return null;
                    }
                }
 
                if (text[i..(i + 1)] == "\"\"")
                {
                    // quote literal
                    valueStrBuilder.Append('"');
                    ++i;
                }
                else if (text[i] == '"')
                {
                    // end quote
                    inQuote = false;
                    continue;
                }
                else
                {
                    // add to value
                    valueStrBuilder.Append(text[i]);
                }
            }
            else
            {
                switch (text[i])
                {
                    case ',':
                        result.rows[^1].Add(valueStrBuilder.ToString());
                        valueStrBuilder.Clear();
                        break;
                    case '\n':
                        result.rows[^1].Add(valueStrBuilder.ToString());
                        valueStrBuilder.Clear();
                        result.rows.Add(new());
                        break;
                    case '"':
                        inQuote = true;
                        break;
                    default:
                        valueStrBuilder.Append(text[i]);
                        break;
                }
            }
        }
 
        // return CsvObject
        return result;
    }
}