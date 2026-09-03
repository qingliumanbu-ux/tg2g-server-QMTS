/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      wencm
Version:     1.0
Date:        2024-07-05 19:54:28
Description: 修磨要求生成
**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT


int f_qmts_grind_desc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString sqlstr1 = " ";
	CDbCommand cmd(conn);
	CDbCommand cmd_1(conn);
	CModel tqmts11("TQMTS11");
	try
	{
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "MEASURE_DESC");
		//修磨要求生成
		CString s_grind_desc ="";
		sqlstr = " SELECT COUNT(*) FROM TQMTS11A WHERE STEEL_GRADE = @steel_grade AND casting_pre_judgment is null and spec_min is null and spec_max is null and use is null";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("steel_grade", bcls_rec->Tables[0].Rows[0]["STEEL_GRADE"].ToString());
		cmd.Parameters.Set("sg_grade_1", bcls_rec->Tables[0].Rows[0]["SG_GRADE_1"].ToString());

		cmd.ExecuteReader();
		if (cmd.Read())
		{
			if (cmd.GetDecimal(1)==1)
			{
				s_grind_desc = "全修";
			}
			else
			{
				sqlstr1 = " SELECT CASTING_PRE_JUDGMENT,SPEC_MIN,SPEC_MAX,USE FROM TQMTS11A WHERE STEEL_GRADE = @steel_grade order by CASTING_PRE_JUDGMENT,SPEC_MIN,SPEC_MAX,USE";
				cmd_1.SetCommandText(sqlstr1);
				cmd_1.Parameters.Set("steel_grade", bcls_rec->Tables[0].Rows[0]["STEEL_GRADE"].ToString());
				cmd_1.Parameters.Set("sg_grade_1", bcls_rec->Tables[0].Rows[0]["SG_GRADE_1"].ToString());
				cmd_1.ExecuteReader();
				while (cmd_1.Read())
				{ 
					if (cmd_1.GetString(1) != " ")
					{
						s_grind_desc += cmd_1.GetString(1) + "/";
					}
					else
					{
						if (cmd_1.GetDecimal(2) > 0)
						{
							if (cmd_1.GetDecimal(3) > 0)
							{
								s_grind_desc += "等于" + cmd_1.GetString(2) + "全修-介于" + cmd_1.GetString(2) + "和" + cmd_1.GetString(3) + "之间边修";
							}
							else
							{
								s_grind_desc += "小于等于" + cmd_1.GetString(2) + "全修";
							}
						}
						else
						{
							if (cmd_1.GetString(4) != " ")
							{ 
								s_grind_desc += cmd_1.GetString(4) + "/";
							}
						}
					}
				}
				s_grind_desc += "全修";
			}
			
		}
		Log::Trace("", "s_grind_desc", "s_grind_desc = { 0 }", s_grind_desc);
		bcls_ret->Tables[0].Rows.Add();
		bcls_ret->Tables[0].Rows[0]["MEASURE_DESC"] = s_grind_desc;
		tqmts11["STEEL_GRADE"] = bcls_rec->Tables[0].Rows[0]["STEEL_GRADE"].ToString();
		tqmts11["SG_GRADE_1"] = bcls_rec->Tables[0].Rows[0]["SG_GRADE_1"].ToString();
		tqmts11["MEASURE_DESC"] = s_grind_desc;
		if (tqmts11.QueryCount("STEEL_GRADE,SG_GRADE_1")){
			tqmts11.Update("MEASURE_DESC", "STEEL_GRADE,SG_GRADE_1");
		}
		else{
			tqmts11.Insert();
		}

	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


