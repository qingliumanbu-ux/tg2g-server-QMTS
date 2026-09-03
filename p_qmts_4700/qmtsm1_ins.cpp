/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      KYAG67
Version:     1.0
Date:        2021-03-16 13:42:24
Description: 静态表-新增
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmtsm1_ins)


int f_qmtsm1_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	/*获取当前时间*/
	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		// ----------------------------------------
		// 程序公共CDbCommand
		CDbCommand cmd(conn);

		/* 申明新增语句 */
		CString sql_ins = "INSERT INTO T";
		/* 获取前台传入的窗体名称 */
		CString form_name = (CString)bcls_rec->Tables[1].Rows[0]["form_name"].ToString().Trim();
		// 同产品大类，牌号分段号，MIC起始号相同的，新增时取相同的MIC当前号
		/* 拼接sql语句 */
		sql_ins += form_name;
		/* 打印表名 */
		Log::Trace("", "", "table_name{0}", (const char*)form_name);

		sql_ins += " ( ";

		/* 获取压入表名称 */
		for (int j = 0; j<bcls_rec->Tables[0].Columns.get_Count(); j++)
		{
			/* 拼接列名 */
			sql_ins += bcls_rec->Tables[0].Columns[j].get_ColumnName();

			/* 判断拼接,若相等的的时候，则拼接此列 */
			if (j == bcls_rec->Tables[0].Columns.get_Count() - 1)
			{
				sql_ins += " ) VALUES ( ";
			}
			else
			{
				sql_ins += ",";
			}
		}

		/* 打印SQL_INS语句 */
		Log::Trace("QXSM", "qmsmm1_ins", "sql_ins[{0}]", (const char*)sql_ins);

		/*获取传入行数*/
		int count = bcls_rec->Tables[0].Rows.get_Count();

		/*循环压入数据表行数*/
		for (int i = 0; i < count; i++)
		{
			/*申明sql值*/
			CString sql_value;

			/*循环列数*/
			for (int j = 0; j < bcls_rec->Tables[0].Columns.get_Count(); j++)
			{
				if (bcls_rec->Tables[0].Columns[j].get_ColumnName() == "REC_CREATOR")
				{
					sql_value += "@rec_creator";
				}
				else if (bcls_rec->Tables[0].Columns[j].get_ColumnName() == "REC_REVISOR")
				{
					sql_value += "@rec_revisor";
				}
				else if (bcls_rec->Tables[0].Columns[j].get_ColumnName() == "REC_CREATE_TIME")
				{
					sql_value += "@rec_create_time";
				}
				else if (bcls_rec->Tables[0].Columns[j].get_ColumnName() == "REC_REVISE_TIME")
				{
					sql_value += "@rec_revise_time";
				}
				/*else if (bcls_rec->Tables[0].Columns[j].get_ColumnName() == "VERSION")
				{
					sql_value += "0";
				}				else if (bcls_rec->Tables[0].Columns[j].get_ColumnName() == "SEND_TIME")
				{
					sql_value += "' '";
				}				else if (bcls_rec->Tables[0].Columns[j].get_ColumnName() == "SEND_MAKER")
				{
					sql_value += "' '";
				}				else if (bcls_rec->Tables[0].Columns[j].get_ColumnName() == "VALID_FLAG")
				{
					sql_value += "' '";
				}*/
				/*else if (bcls_rec->Tables[0].Columns[j].get_ColumnName() == "DU_FLAG")
				{
				sql_value += "' '";
				}
				else if (bcls_rec->Tables[0].Columns[j].get_ColumnName() == "DU_MAKER")
				{
				sql_value += "' '";
				}
				else if (bcls_rec->Tables[0].Columns[j].get_ColumnName() == "DU_TIME")
				{
				sql_value += "' '";
				}*/
				else
				{					Log::Trace("", "", "*{0}*", (const char*)((CString)bcls_rec->Tables[0].Rows[i][j]).Trim());
					if (((CString)bcls_rec->Tables[0].Rows[i][j]).IsEmpty())
					{
						bcls_rec->Tables[0].Rows[i][j] = " ";
						Log::Trace("", "", "****{0}****", (const char*)bcls_rec->Tables[0].Rows[i][j].ToString());
					}
					sql_value += "'" + ((CString)bcls_rec->Tables[0].Rows[i][j]).Replace("'", "") + "'";
				}
				if (j == bcls_rec->Tables[0].Columns.get_Count() - 1)
				{
					sql_value += " ) ";
				}
				else
				{
					sql_value += ",";
				}
			}
			/*拼接成完整的的sql语句*/
			CString sql = sql_ins + sql_value;
			/* 打印sql语句 */
			Log::Trace("", "", "sql{0}", (const char*)sql);

			/* 连接数据库，对在线表进行操作 */
			CDbCommand comm2(sql, conn);
			comm2.Parameters.Set("rec_creator", s.userid);
			comm2.Parameters.Set("rec_revisor", s.userid);
			comm2.Parameters.Set("rec_create_time", datetimeNow);
			comm2.Parameters.Set("rec_revise_time", datetimeNow);
			comm2.ExecuteNonQuery();
		}
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.sqlcode = ex.GetCode();
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


