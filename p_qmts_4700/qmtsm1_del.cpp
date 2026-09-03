/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      KYAG67
Version:     1.0
Date:        2021-03-16 13:44:31
Description: 静态表-删除
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmtsm1_del)
#if !defined _SYS_PES	//PES
//int f_m1p1qf_snd(EIClass * bcls_rec, CString tc_id, CDbConnection * conn);
#endif
int f_qmtsm1_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	// 系统日志类定义
	CTracer log(__FUNCTION__);

	// 程序内部用变量
	int         doFlag = 0;
	int         logFlag = 1;

	// 数据库SQL操作字符串，用于捕获数据库操作异常情况
	CString     sqlstr = "";
	CString     datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");


	// ------------------------------------------------------------
	// 程序开始
	try
	{
		// ---------------------------------------- 
		// 定义公共CDbCommand
		CDbCommand  cmd(conn);

		// 获取要前台传入的值
		CString from_name = (CString)bcls_rec->Tables[1].Rows[0]["form_name"];
		CString t_only = (CString)bcls_rec->Tables[1].Rows[0]["t_only"];

		// 新增历史表记录 后续需要的可以自行放开
		//CString sql_ins = "INSERT INTO H" + from_name + " (";

		//// 取得单行传入信息
		//for (int j = 0; j < bcls_rec->Tables[0].Columns.get_Count(); j++)
		//{
		//	sql_ins += bcls_rec->Tables[0].Columns[j].get_ColumnName();

		//	if (j == bcls_rec->Tables[0].Columns.get_Count() - 1)
		//	{
		//		sql_ins += " ) VALUES ( ";
		//	}
		//	else
		//	{
		//		sql_ins += " , ";
		//	}
		//}


		// 获取要删除的行数，逐行进行处理
		int count = bcls_rec->Tables[0].Rows.get_Count();
		for (int i = 0; i < count; i++)
		{
			CString sql_value;
			// 删除SQL语句
			CString sql_del = "DELETE FROM T" + from_name + " WHERE 1 = 1 ";

			// 先逐列新增入历史表
			for (int j = 0; j < bcls_rec->Tables[0].Columns.get_Count(); j++)
			{
				if (bcls_rec->Tables[0].Columns[j].get_ColumnName() == "DU_MAKER")
				{
					sql_value += "@du_maker";
				}
				else if (bcls_rec->Tables[0].Columns[j].get_ColumnName() == "DU_TIME")
				{
					sql_value += "@du_time";
				}
				else if (bcls_rec->Tables[0].Columns[j].get_ColumnName() == "DU_FLAG")
				{
					sql_value += "'D'";
				}
				else
				{
					sql_value += " @" + bcls_rec->Tables[0].Columns[j].get_ColumnName();
					cmd.Parameters.Set(bcls_rec->Tables[0].Columns[j].get_ColumnName(), (CString)bcls_rec->Tables[0].Rows[i][j].ToString().TrimOrBlank());
				}

				if (j == bcls_rec->Tables[0].Columns.get_Count() - 1)
				{
					sql_value += " ) ";
				}
				else
				{
					sql_value += " , ";
				}
			}
			//sqlstr = sql_ins + sql_value;

			//// 执行历史表新增操作
			//cmd.SetCommandText(sqlstr);
			//cmd.Parameters.Set("du_maker", s.userid);
			//cmd.Parameters.Set("du_time", datetimeNow);
			//if (t_only.Trim() == "")
			//{
			//	cmd.ExecuteNonQuery();
			//}
			//cmd.Close();

			// 根据前台传入主键删除
			for (int k = 0; k < bcls_rec->Tables[2].Rows.get_Count(); k++)
			{
				if (bcls_rec->Tables[0].Rows[i][bcls_rec->Tables[2].Rows[k]["COLNAME"].ToString().Trim()].ToString().Trim() != "")
				{
					sql_del += " AND " + bcls_rec->Tables[2].Rows[k]["COLNAME"].ToString().Trim() + "=@colName_" + CConvert::ToString(k);
					cmd.Parameters.Set("colName_" + CConvert::ToString(k), (CString)bcls_rec->Tables[0].Rows[i][bcls_rec->Tables[2].Rows[k]["COLNAME"].ToString().Trim()].ToString().Trim());
				}
			}

			// 执行在线表删除操作
			sqlstr = sql_del;
			cmd.SetCommandText(sqlstr);
			Log::Trace("", "", "sql[{0}][{1}]", sqlstr, bcls_rec->Tables[0].Rows[i][bcls_rec->Tables[2].Rows[0]["COLNAME"].ToString().Trim()].ToString().Trim());
			//cmd.SetCommandText(sql_del);
			cmd.ExecuteNonQuery();
			//发送删除电文。目前做的校验比较简单，判断列名中是否有发送时间列(说明是需要下发的基表)，如果有就发送电文。
			
		}
		/*if (bcls_rec->Tables[0].Columns.Contains("SEND_TIME"))
		{
			CString base_div = "";
			if (from_name != "QMTS43" && from_name != "QMTS44" && from_name != "QMBMS6") //除了这三张，其余直接发给本部五个基地
			{
				base_div = "A";
			}
			bool factory_div_diff = 0;
			if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))
			{
				factory_div_diff = 1;
			}
			EIClass		table_rec_a31;
			EIClass		table_rec_a32;
			EIClass		table_rec_a33;
			EIClass		table_rec_a34;
			EIClass		table_rec_a35;
			EIClass		table_rec_b31;
			EIClass		table_rec_c31;
			table_rec_a31.Tables[0].set_TableName("D_T" + from_name);
			table_rec_a32.Tables[0].set_TableName("D_T" + from_name);
			table_rec_a33.Tables[0].set_TableName("D_T" + from_name);
			table_rec_a34.Tables[0].set_TableName("D_T" + from_name);
			table_rec_a35.Tables[0].set_TableName("D_T" + from_name);
			table_rec_b31.Tables[0].set_TableName("D_T" + from_name);
			table_rec_c31.Tables[0].set_TableName("D_T" + from_name);

			for (size_t i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				if (i == 0)
				{
					table_rec_a31.Tables[0].Clone(bcls_rec->Tables[0]);
					table_rec_a32.Tables[0].Clone(bcls_rec->Tables[0]);
					table_rec_a33.Tables[0].Clone(bcls_rec->Tables[0]);
					table_rec_a34.Tables[0].Clone(bcls_rec->Tables[0]);
					table_rec_a35.Tables[0].Clone(bcls_rec->Tables[0]);
					table_rec_b31.Tables[0].Clone(bcls_rec->Tables[0]);
					table_rec_c31.Tables[0].Clone(bcls_rec->Tables[0]);
				}
				if (base_div == "A")
				{
					if (factory_div_diff)
					{
						CString factory_div = bcls_rec->Tables[0].Rows[i]["FACTORY_DIV"];
						if (factory_div == "A31")
						{
							table_rec_a31.Tables[0].Rows.Add();
							table_rec_a31.Tables[0].Rows[table_rec_a31.Tables[0].Rows.get_Count() - 1].Merge(bcls_rec->Tables[0].Rows[i]);
						}
						else if (factory_div == "A32")
						{
							table_rec_a32.Tables[0].Rows.Add();
							table_rec_a32.Tables[0].Rows[table_rec_a32.Tables[0].Rows.get_Count() - 1].Merge(bcls_rec->Tables[0].Rows[i]);
						}
						else if (factory_div == "A33")
						{
							table_rec_a33.Tables[0].Rows.Add();
							table_rec_a33.Tables[0].Rows[table_rec_a33.Tables[0].Rows.get_Count() - 1].Merge(bcls_rec->Tables[0].Rows[i]);
						}
						else if (factory_div == "A34")
						{
							table_rec_a34.Tables[0].Rows.Add();
							table_rec_a34.Tables[0].Rows[table_rec_a34.Tables[0].Rows.get_Count() - 1].Merge(bcls_rec->Tables[0].Rows[i]);
						}
						else if (factory_div == "A35")
						{
							table_rec_a35.Tables[0].Rows.Add();
							table_rec_a35.Tables[0].Rows[table_rec_a35.Tables[0].Rows.get_Count() - 1].Merge(bcls_rec->Tables[0].Rows[i]);
						}
					}
					else
					{
						table_rec_a31.Tables[0].Rows.Add();
						table_rec_a31.Tables[0].Rows[i].Merge(bcls_rec->Tables[0].Rows[i]);
						table_rec_a32.Tables[0].Rows.Add();
						table_rec_a32.Tables[0].Rows[i].Merge(bcls_rec->Tables[0].Rows[i]);
						table_rec_a33.Tables[0].Rows.Add();
						table_rec_a33.Tables[0].Rows[i].Merge(bcls_rec->Tables[0].Rows[i]);
						table_rec_a34.Tables[0].Rows.Add();
						table_rec_a34.Tables[0].Rows[i].Merge(bcls_rec->Tables[0].Rows[i]);
						table_rec_a35.Tables[0].Rows.Add();
						table_rec_a35.Tables[0].Rows[i].Merge(bcls_rec->Tables[0].Rows[i]);
					}
				}
				else
				{
					CString factory_div = bcls_rec->Tables[0].Rows[i]["FACTORY_DIV"];
					if (factory_div == "A31")
					{
						table_rec_a31.Tables[0].Rows.Add();
						table_rec_a31.Tables[0].Rows[table_rec_a31.Tables[0].Rows.get_Count() - 1].Merge(bcls_rec->Tables[0].Rows[i]);
					}
					else if (factory_div == "A32")
					{
						table_rec_a32.Tables[0].Rows.Add();
						table_rec_a32.Tables[0].Rows[table_rec_a32.Tables[0].Rows.get_Count() - 1].Merge(bcls_rec->Tables[0].Rows[i]);
					}
					else if (factory_div == "A33")
					{
						table_rec_a33.Tables[0].Rows.Add();
						table_rec_a33.Tables[0].Rows[table_rec_a33.Tables[0].Rows.get_Count() - 1].Merge(bcls_rec->Tables[0].Rows[i]);
					}
					else if (factory_div == "A34")
					{
						table_rec_a34.Tables[0].Rows.Add();
						table_rec_a34.Tables[0].Rows[table_rec_a34.Tables[0].Rows.get_Count() - 1].Merge(bcls_rec->Tables[0].Rows[i]);
					}
					else if (factory_div == "A35")
					{
						table_rec_a35.Tables[0].Rows.Add();
						table_rec_a35.Tables[0].Rows[table_rec_a35.Tables[0].Rows.get_Count() - 1].Merge(bcls_rec->Tables[0].Rows[i]);
					}
					else if (factory_div == "B31")
					{
						table_rec_b31.Tables[0].Rows.Add();
						table_rec_b31.Tables[0].Rows[table_rec_b31.Tables[0].Rows.get_Count() - 1].Merge(bcls_rec->Tables[0].Rows[i]);
					}
					else if (factory_div == "C31")
					{
						table_rec_c31.Tables[0].Rows.Add();
						table_rec_c31.Tables[0].Rows[table_rec_c31.Tables[0].Rows.get_Count() - 1].Merge(bcls_rec->Tables[0].Rows[i]);
					}
				}
			}
#if !defined _SYS_PES	//PES
			if (table_rec_a31.Tables[0].Rows.get_Count() > 0)
			{
				table_rec_a31.Tables.Add(bcls_rec->Tables[2]);
				//doFlag = f_m1p1qf_snd(&table_rec_a31, "M1P1QF", conn);
				if (doFlag < 0)
				{
					//sprintf(s.sysmsg, "调用 f_m1p1qf_snd 函数出错");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			if (table_rec_a32.Tables[0].Rows.get_Count() > 0)
			{
				table_rec_a32.Tables.Add(bcls_rec->Tables[2]);
				//doFlag = f_m1p1qf_snd(&table_rec_a32, "M1P2QF", conn);
				if (doFlag < 0)
				{
					//sprintf(s.sysmsg, "调用 f_m1p1qf_snd 函数出错");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			if (table_rec_a33.Tables[0].Rows.get_Count() > 0)
			{
				table_rec_a33.Tables.Add(bcls_rec->Tables[2]);
				//doFlag = f_m1p1qf_snd(&table_rec_a33, "M1P3QF", conn);
				if (doFlag < 0)
				{
					//sprintf(s.sysmsg, "调用 f_m1p1qf_snd 函数出错");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			if (table_rec_a34.Tables[0].Rows.get_Count() > 0)
			{
				table_rec_a34.Tables.Add(bcls_rec->Tables[2]);
				//doFlag = f_m1p1qf_snd(&table_rec_a34, "M1P5QF", conn);
				if (doFlag < 0)
				{
					//sprintf(s.sysmsg, "调用 f_m1p1qf_snd 函数出错");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			if (table_rec_a35.Tables[0].Rows.get_Count() > 0)
			{
				table_rec_a35.Tables.Add(bcls_rec->Tables[2]);
				//doFlag = f_m1p1qf_snd(&table_rec_a35, "M1P4QF", conn);
				if (doFlag < 0)
				{
					//sprintf(s.sysmsg, "调用 f_m1p1qf_snd 函数出错");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			if (table_rec_b31.Tables[0].Rows.get_Count() > 0)
			{
				table_rec_b31.Tables.Add(bcls_rec->Tables[2]);
				//doFlag = f_m1p1qf_snd(&table_rec_b31, "M1PAQF", conn);
				if (doFlag < 0)
				{
					//sprintf(s.sysmsg, "调用 f_m1p1qf_snd 函数出错");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			if (table_rec_c31.Tables[0].Rows.get_Count() > 0)
			{
				table_rec_c31.Tables.Add(bcls_rec->Tables[2]);
				//doFlag = f_m1p1qf_snd(&table_rec_c31, "M1PCQF", conn);
				if (doFlag < 0)
				{
					//sprintf(s.sysmsg, "调用 f_m1p1qf_snd 函数出错");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
#endif
			if (doFlag < 0)
			{
				sprintf(s.sysmsg, "调用 f_m1p1qf_snd 函数出错");
				throw CApplicationException(-1, s.msg, log.Location);
			}

		}*/


		strcpy(s.msg, _RES("GCRSS0000002")/*处理成功*/);
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


