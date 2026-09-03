/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      KYAG67
Version:     1.0
Date:        2021-03-16 13:43:09
Description: 静态表-修改
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmtsm1_upd)
#if !defined _SYS_PES	//PES
//int f_m1p1qf_snd(EIClass * bcls_rec, CString tc_id, CDbConnection * conn);
#endif
int f_qmtsm1_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int  doFlag = 0;
	int k = 0;

	/*	数据库SQL操作字符串，用于捕获数据库操作异常情况	*/
	CString sqlstr = "";
	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	// ---------------------------------------- 
	// 定义公共CDbcmdand
	CDbCommand  cmd(conn);


	try
	{
		Log::Trace("", "", "update start");
		//-----------------------------------------------
		// 获取要操作的行前台传入的行数
		int count = bcls_rec->Tables[0].Rows.get_Count();
		CString	formName = (CString)bcls_rec->Tables[1].Rows[0]["form_name"].ToString().Trim();
		CString t_only = (CString)bcls_rec->Tables[1].Rows[0]["t_only"];

		//-----------------------------------------------
		// 循环获取列名和列值，插入历史表
		for (int i = 0; i < count; i++)
		{


			CString sql_ins = "INSERT INTO H" + formName + " ( ";
			//要插入历史表的值
			CString ins_value = "";

			// 循环获取列名
			for (int j = 0; j < bcls_rec->Tables[0].Columns.get_Count(); j++)
			{

				sql_ins += bcls_rec->Tables[0].Columns[j].get_ColumnName();

				if (j == bcls_rec->Tables[0].Columns.get_Count() - 1)
				{
					sql_ins += " ) VALUES ( ";
				}
				else
				{
					sql_ins += ",";
				}
			}

			// 循环获取要插入的值
			for (int k = 0; k < bcls_rec->Tables[0].Columns.get_Count(); k++)
			{

				// 历史表数据修改sql拼接
				if (bcls_rec->Tables[0].Columns[k].get_ColumnName() == "DU_MAKER")
				{
					ins_value += "@du_maker";
				}
				else if (bcls_rec->Tables[0].Columns[k].get_ColumnName() == "DU_TIME")
				{
					ins_value += "@du_time";
				}
				else if (bcls_rec->Tables[0].Columns[k].get_ColumnName() == "DU_FLAG")
				{
					ins_value += "'U'";
				}
				/*else if (bcls_rec->Tables[0].Columns[k].get_ColumnName() == "VERSION")
				{
					ins_value += "@version";
				}*/
				else
				{
					ins_value += "'" + (CString)bcls_rec->Tables[0].Rows[i][k].ToString().TrimOrBlank() + "'";
				}

				if (k == bcls_rec->Tables[0].Columns.get_Count() - 1)
				{
					ins_value += " ) ";
				}
				else
				{
					ins_value += ",";
				}
			}

			sqlstr = sql_ins + ins_value;
			//LogCS(sqlstr);
			Log::Trace("", "", "HIS_INSERT sqlstr{0}", sqlstr);

			cmd.SetCommandText(sqlstr);
			cmd.Parameters.Set("du_maker", s.userid);
			cmd.Parameters.Set("du_time", datetimeNow);
			cmd.Parameters.Set("version", (CDecimal)bcls_rec->Tables[0].Rows[i]["VERSION"] + 1);
			if (t_only.Trim() == "")
			{
				cmd.ExecuteNonQuery();
			}
			cmd.Close();

		}
		//-----------------------------------------------
		// 循环更新在线表
		for (int i = 0; i < count; i++)
		{
			// 插入历史表的SQL语句
			CString		sql_upd = "UPDATE T" + formName + " SET ";

			for (int j = 0; j < bcls_rec->Tables[0].Columns.get_Count(); j++)
			{
				// 在线表数据修改sql拼接 
				if (bcls_rec->Tables[0].Columns[j].get_ColumnName() == "REC_REVISOR")
				{
					sql_upd += bcls_rec->Tables[0].Columns[j].get_ColumnName() + " = @rec_revisor,";
				}
				else if (bcls_rec->Tables[0].Columns[j].get_ColumnName() == "REC_REVISE_TIME")
				{
					sql_upd += bcls_rec->Tables[0].Columns[j].get_ColumnName() + " = @rec_revise_time,";
				}
				//else if (bcls_rec->Tables[0].Columns[j].get_ColumnName() == "VERSION")
				//{
				//	sql_upd += bcls_rec->Tables[0].Columns[j].get_ColumnName() + " = @version,";
				//}
				//else if (bcls_rec->Tables[0].Columns[j].get_ColumnName() == "SEND_TIME")//更新SEND_TIME,SEND_MAKER,VALID_FLAG为空
				//{
				//	sql_upd += bcls_rec->Tables[0].Columns[j].get_ColumnName() + " = ' ',";
				//}
				//else if (bcls_rec->Tables[0].Columns[j].get_ColumnName() == "SEND_MAKER")//更新SEND_TIME,SEND_MAKER,VALID_FLAG为空
				//{
				//	sql_upd += bcls_rec->Tables[0].Columns[j].get_ColumnName() + " = ' ',";
				//}
				//else if (bcls_rec->Tables[0].Columns[j].get_ColumnName() == "VALID_FLAG")//更新SEND_TIME,SEND_MAKER,VALID_FLAG为空
				//{
				//	sql_upd += bcls_rec->Tables[0].Columns[j].get_ColumnName() + " = ' ',";
				//}
				else
				{
					sql_upd += bcls_rec->Tables[0].Columns[j].get_ColumnName() + " = '" + (CString)bcls_rec->Tables[0].Rows[i][j].ToString().TrimOrBlank() + "',";
				}

				// sql更新的条件根据主键更新
				if (j == bcls_rec->Tables[0].Columns.get_Count() - 1)
				{
					// 截取字符串的最后一位的逗号
					sql_upd = sql_upd.Substring(0, sql_upd.GetLength() - 1);

					for (int k = 0; k < bcls_rec->Tables[2].Rows.get_Count(); k++)
					{
						if (0 == k)
						{
							sql_upd += " WHERE " + bcls_rec->Tables[2].Rows[k]["COLNAME"].ToString().Trim() + "=@colName_" + CConvert::ToString(k);
						}
						else
						{
							sql_upd += " AND " + bcls_rec->Tables[2].Rows[k]["COLNAME"].ToString().Trim() + "=@colName_" + CConvert::ToString(k);
						}

						cmd.Parameters.Set("colName_" + CConvert::ToString(k), (CString)bcls_rec->Tables[0].Rows[i][bcls_rec->Tables[2].Rows[k]["COLNAME"].ToString().Trim()].ToString().Trim());
					}
				}
			}

			//更新在线表
			sqlstr = sql_upd;
			Log::Trace("", "", "UPDATE sql_upd{0}", sqlstr);

			cmd.SetCommandText(sqlstr);
			cmd.Parameters.Set("rec_revisor", s.userid);
			cmd.Parameters.Set("rec_revise_time", datetimeNow);
			cmd.Parameters.Set("version", (CDecimal)bcls_rec->Tables[0].Rows[i]["VERSION"] + 1);
			cmd.ExecuteNonQuery();

			//发送删除电文。目前做的校验比较简单，判断列名中是否有发送时间列(说明是需要下发的字段)，如果有就发宋电文。
			
		}
		/*if (bcls_rec->Tables[0].Columns.Contains("SEND_TIME"))
		{
			CString base_div = "";
			if (formName != "QMTS43" && formName != "QMTS44" && formName != "QMBMS6") //除了这三张，其余直接发给本部五个基地
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
			table_rec_a31.Tables[0].set_TableName("D_T" + formName);
			table_rec_a32.Tables[0].set_TableName("D_T" + formName);
			table_rec_a33.Tables[0].set_TableName("D_T" + formName);
			table_rec_a34.Tables[0].set_TableName("D_T" + formName);
			table_rec_a35.Tables[0].set_TableName("D_T" + formName);
			table_rec_b31.Tables[0].set_TableName("D_T" + formName);
			table_rec_c31.Tables[0].set_TableName("D_T" + formName);

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


