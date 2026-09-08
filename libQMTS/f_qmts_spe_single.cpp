/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      179412
Version:     1.0
Date:        2016-07-29
Description: 成分组合元素计算函数(综判时调用)
**************************************************/


/*
_ooOoo_
o8888888o
88" . "88
(| -_- |)
O\  =  /O
____/`---'\____
.'  \\|     |//  `.
/  \\|||  :  |||//  \
/  _||||| -:- |||||-  \
|   | \\\  -  /// |   |
| \_|  ''\---/''  |   |
\  .-\__  `-`  ___/-. /
___`. .'  /--.--\  `. . __
."" '<  `.___\_<|>_/___.'  >'"".
| | :  `- \`.;`\ _ /`;.`/ - ` : | |
\  \ `-.   \_ __\ /__ _/   .-` /  /
======`-.____`-.___\_____/___.-`____.-'======
`=---='
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
佛祖保佑       永无BUG
*/



//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件，请包含在""中

#include "math.h"


BM2_FUNCTION_EXPORT
int f_qmts_spe_single(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义


	//程序用变量
	int			doFlag = 0;
	int			i = 0;
	int			j = 0;
	int			fetchRowCount = 0;
	CString		sql_replace = " ";	
	CString		s_table_name = "";	//表名
	CString		s_msc = "";
	CString		s_order_no = "";
	CString		s_sample_lot_no = "";
	CString		s_pono = ""; 
	CString     v_heat_no = " ";//熔炼号
	CString		s_elm_code_1 = "";
	CString		s_elm_fmla_code_1 = "";
	CString		s_elm_fmla_code_ceq = "";
	CString		s_elm_fmla_code_pcm = "";
	CString		s_rec_revisor = s.userid;
	CString		s_rec_revise_time = CDateTime::Now().ToString("yyyyMMddHHmmss");
	int         v_htno_flag = 0;//实绩来源标志:2－按PONO号判定成分;1－按熔炼号判定成分(预留，目前没用)
	CString		formula_value = "";	//实绩公式
	CString		formula_value_replace_after = "";	//替换元素后实绩公式
	CString		formula_std = "";	//标准公式
	CString		formula_std_replace_after = "";	//替换元素后标准公式
	CString		symbol = "";	//符号/条件
	CString		symbol_replace_after = "";	//替换元素后符号/条件公式
	CDecimal	symbol_val = 0;	//条件计算结果

	CString		s_elm_code = "";
	CString		s_elm_code_name = "";
	CString		s_elm_name = "";
	CDecimal	s_elm_value = 0;
	CString    s_elm_name_q0 = "";
	CString		elm_null = "";	//公式替换后未找到实绩的元素

	CString		sqlstr = "";
	CString		sqlstr_q3 = "";
	CString	    sqlstr_elm_code = "";
	CString     s_elm_act_value = " ";
	/* 实体类定义 */
	CModel tqmtqq0("TQMTQQ0");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_upd(conn);
	CDbCommand cmd_ins(conn);
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_q3(conn);
	CDbCommand cmd_inq_01(conn);
	CDbCommand cmd_inq_qmys(conn);
	CDbCommand cmd_inq_elm(conn);
	CDbCommand cmd_inq_elm_code(conn);
	CDbCommand tmmsm01_inq(conn);

	CDbCommand cmd_inq_021(conn);
	CDbCommand cmd_inq_02(conn);
	CDbCommand cmd_inq_03(conn);

	//输出块
	if (!bcls_ret->Tables[0].Columns.Contains("ELM_FMLA_VALUE"))
	{
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "ELM_FMLA_VALUE");
	}
	bcls_ret->Tables[0].Rows.Add();

	try
	{
		/*获得传入参数*/

		s_elm_fmla_code_1 = bcls_rec->Tables["QMTS_SPE"].Rows[0]["ELM_FMLA_CODE"].ToString().TrimOrBlank().ToUpper();
		Log::Trace("", __FUNCTION__, "s_elm_fmla_code_1[{0}]", s_elm_fmla_code_1);

		//校验传入参数
		if (" " == s_elm_fmla_code_1)
		{
			Log::Trace("", __FUNCTION__, "ERROR------[传入参数ELM_FMLA_CODE不允许为空]");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//获得组合元素公式代码
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT CODE_DESC_1_CONTENT,UPPER(CODE_DESC_2_CONTENT),CODE_DESC_3_CONTENT "
				"   FROM TEP0002 "
				"  WHERE CODE_CLASS = 'QM63' "
				"    AND CODE = @code ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("code", s_elm_fmla_code_1);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			formula_std = cmd_inq.GetString(1);
			formula_value = cmd_inq.GetString(2);
			symbol = cmd_inq.GetString(3);

			Log::Trace("", __FUNCTION__, "符号[{0}], 实绩公式[{1}], 标准公式[{2}]", symbol, formula_value, formula_std);

			if (formula_value.TrimOrBlank() != " ")
			{
				Log::Trace("", __FUNCTION__, "实绩公式formula_value 计算开始");
				Log::Trace("", __FUNCTION__, "转换绝对值符号");

				double j = 0;

				for (i = 1; i <= formula_value.GetLength(); i++)
				{
					if ("|" == formula_value.SubstringNE(i - 1, 1))
					{
						j = j + 1;

						Log::Trace("", __FUNCTION__, "实绩公式第i[{0}]位, 第j[{1}]个'|'", i, j);

						if (0 == fmod(j, 2))
						{
							Log::Trace("", __FUNCTION__, "双数");

							switch (conn->DatabaseKind)
							{
							case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
// DM8 适配 CHANGE-188:查询。SYSIBM 辅助表改为 DUAL。
// 改写原因：SYSIBM 辅助表改为 DUAL；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
								// sqlstr = " SELECT substr(@formula_value,1,@i-1) || ')' || substr(@formula_value,@i+1) FROM SYSIBM.DUAL  ";
// DM8 SQL：
								sqlstr = " SELECT substr(@formula_value,1,@i-1) || ')' || substr(@formula_value,@i+1) FROM DUAL  ";
								break;
							case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
							case DB_KIND_MSSQL:				// MS SQL Server数据库
							case DB_KIND_ORACLE:	        // Oracle 数据库
								sqlstr = " SELECT substr(@formula_value,1,@i-1) || ')' || substr(@formula_value,@i+1) FROM DUAL ";
								break;
							default:						// 所有数据库适用，通用SQL语句
// DM8 适配 CHANGE-189:查询。SYSIBM 辅助表改为 DUAL。
// 改写原因：SYSIBM 辅助表改为 DUAL；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
								// sqlstr = " SELECT substr(@formula_value,1,@i-1) || ')' || substr(@formula_value,@i+1) FROM SYSIBM.DUAL ";
// DM8 SQL：
								sqlstr = " SELECT substr(@formula_value,1,@i-1) || ')' || substr(@formula_value,@i+1) FROM DUAL ";
								break;
							}
							cmd_inq_01.SetCommandText(sqlstr);
							cmd_inq_01.Parameters.Set("formula_value", formula_value);
							cmd_inq_01.Parameters.Set("i", i);
							cmd_inq_01.ExecuteReader();
							if (cmd_inq_01.Read())
							{
								formula_value = cmd_inq_01.GetString(1);
							}
							cmd_inq_01.Close();

							Log::Trace("", __FUNCTION__, "双数| 实绩公式[{0}]", formula_value);
						}
						else
						{
							Log::Trace("", __FUNCTION__, "单数");

							switch (conn->DatabaseKind)
							{
							case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
// DM8 适配 CHANGE-190:查询。SYSIBM 辅助表改为 DUAL。
// 改写原因：SYSIBM 辅助表改为 DUAL；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
								// sqlstr = " SELECT substr(@formula_value,1,@i-1) || 'abs(' || substr(@formula_value,@i+1) FROM SYSIBM.DUAL  ";
// DM8 SQL：
								sqlstr = " SELECT substr(@formula_value,1,@i-1) || 'abs(' || substr(@formula_value,@i+1) FROM DUAL  ";
								break;
							case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
							case DB_KIND_MSSQL:				// MS SQL Server数据库
							case DB_KIND_ORACLE:	        // Oracle 数据库
								sqlstr = " SELECT substr(@formula_value,1,@i-1) || 'abs(' || substr(@formula_value,@i+1) FROM DUAL ";
								break;
							default:						// 所有数据库适用，通用SQL语句
// DM8 适配 CHANGE-191:查询。SYSIBM 辅助表改为 DUAL。
// 改写原因：SYSIBM 辅助表改为 DUAL；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
								// sqlstr = " SELECT substr(@formula_value,1,@i-1) || 'abs(' || substr(@formula_value,@i+1) FROM SYSIBM.DUAL ";
// DM8 SQL：
								sqlstr = " SELECT substr(@formula_value,1,@i-1) || 'abs(' || substr(@formula_value,@i+1) FROM DUAL ";
								break;
							}
							cmd_inq_01.SetCommandText(sqlstr);
							cmd_inq_01.Parameters.Set("formula_value", formula_value);
							cmd_inq_01.Parameters.Set("i", i);
							cmd_inq_01.ExecuteReader();
							if (cmd_inq_01.Read())
							{
								formula_value = cmd_inq_01.GetString(1);
							}
							cmd_inq_01.Close();

							Log::Trace("", __FUNCTION__, "单数| 实绩公式[{0}]", formula_value);
						}
					}
				}

				Log::Trace("", __FUNCTION__, "111转换绝对值符号后，实绩公式[{0}]", formula_value);

				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " SELECT CODE,CODE_DESC_1_CONTENT "
						"   FROM TEP0002 "
						"  WHERE CODE_CLASS = 'QMYS' "
						"  ORDER BY CODE_DESC_2_CONTENT  ";/*根据元素英文名长度倒排序*/
					break;
				}
				cmd_inq_qmys.SetCommandText(sqlstr);
				cmd_inq_qmys.ExecuteReader();
				while (cmd_inq_qmys.Read())
				{
					s_elm_code = cmd_inq_qmys.GetString(1);
					s_elm_code_name = cmd_inq_qmys.GetString(2).ToUpper();

					//Log::Trace("", __FUNCTION__, "*********************************************************************************");
					Log::Trace("", __FUNCTION__, "s_elm_code[{0}],s_elm_code_name[{1}]", s_elm_code, s_elm_code_name);

					 
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
// DM8 适配 CHANGE-192:查询。SYSIBM 辅助表改为 DUAL。
// 改写原因：SYSIBM 辅助表改为 DUAL；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
						// sqlstr = " SELECT replace(@formula_value,@elm_name,'') FROM SYSIBM.DUAL  ";
// DM8 SQL：
						sqlstr = " SELECT replace(@formula_value,@elm_name,'') FROM DUAL  ";
						break;
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
						sqlstr = " SELECT replace(@formula_value,@elm_name,'') FROM DUAL ";
						break;
					default:						// 所有数据库适用，通用SQL语句
// DM8 适配 CHANGE-193:查询。SYSIBM 辅助表改为 DUAL。
// 改写原因：SYSIBM 辅助表改为 DUAL；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
						// sqlstr = " SELECT replace(@formula_value,@elm_name,'') FROM SYSIBM.DUAL ";
// DM8 SQL：
						sqlstr = " SELECT replace(@formula_value,@elm_name,'') FROM DUAL ";
						break;
					}
					cmd_inq_021.SetCommandText(sqlstr);
					cmd_inq_021.Parameters.Set("formula_value", formula_value);
					cmd_inq_021.Parameters.Set("elm_name", s_elm_code_name.ToUpper());
					cmd_inq_021.ExecuteReader();
					//Log::Trace("", __FUNCTION__, "替代前formula_value[{0}]", formula_value);
					if (cmd_inq_021.Read())
					{
						formula_value_replace_after = cmd_inq_021.GetString(1);
					}
					cmd_inq_021.Close();
					Log::Trace("", __FUNCTION__, "替代后formula_value_replace_after[{0}]formula_value[{1}]", formula_value_replace_after,formula_value);

					/*若元素在公式中存在，查找是否存在实绩数据*/
					s_elm_name = " ";
					if (formula_value_replace_after != formula_value)
					{
						Log::Trace("", __FUNCTION__, "111s_table_name[{0}]s_elm_code[{1}]pono[{2}]", s_table_name, s_elm_code, s_pono);
						s_elm_act_value = " ";
						for (j = 0; j < bcls_rec->Tables["QMTS_SPE"].Rows.get_Count(); j++)
						{
							tqmtqq0["ELM_CODE"] = bcls_rec->Tables["QMTS_SPE"].Rows[j]["ELM_CODE"].ToString().TrimOrBlank().ToUpper();
							s_elm_act_value = bcls_rec->Tables["QMTS_SPE"].Rows[j]["ELM_ACT"].ToString();
							tqmtqq0["ELM_NAME"] = bcls_rec->Tables["QMTS_SPE"].Rows[j]["ELM_NAME"].ToString().TrimOrBlank().ToUpper();

							Log::Trace("", __FUNCTION__, "tqmtqq0.ELM_CODE[{0}]", tqmtqq0["ELM_CODE"].ToString());
							Log::Trace("", __FUNCTION__, "s_elm_act_value[{0}]", s_elm_act_value);
							Log::Trace("", __FUNCTION__, "tqmtqq0.ELM_NAME[{0}]", tqmtqq0["ELM_NAME"].ToString());
							if (s_elm_code == tqmtqq0["ELM_CODE"].ToString())
							{
								s_elm_name = tqmtqq0["ELM_NAME"];

								Log::Trace("", __FUNCTION__, "formula_value[{0}]s_elm_name[{1}]", formula_value, s_elm_name);
								/*sql_replace = " SELECT replace(@formula_value,@s_elm_name,@s_elm_act_value) FROM DUAL ";
								cmd_inq_02.SetCommandText(sql_replace);
								cmd_inq_02.Parameters.Set("formula_value", formula_value);
								cmd_inq_02.Parameters.Set("s_elm_name", s_elm_name);
								cmd_inq_02.Parameters.Set("s_elm_act_value", s_elm_act_value);
								Log::Trace("", __FUNCTION__, "sql_replace[{0}]", sql_replace);
								cmd_inq_02.ExecuteReader();
								Log::Trace("", __FUNCTION__, "2sql_replace[{0}]", sql_replace);
								if (cmd_inq_02.Read())
								{
									formula_value = cmd_inq_02.GetString(1);
									Log::Trace("", __FUNCTION__, "formula_value[{0}]", formula_value);
								}
								cmd_inq_02.Close();*/
								formula_value=formula_value.Replace(s_elm_name, s_elm_act_value);
								Log::Trace("", __FUNCTION__, "222formula_value[{0}]", formula_value);
							}

						}//for (j = 0; j < bcls_rec->Tables["QMTS_SPE"].Rows.get_Count(); j++)
						if (s_elm_name.TrimOrBlank() == " ")
						{
							elm_null = elm_null + "," + s_elm_code_name.Trim();
							Log::Trace("", __FUNCTION__, "1-未找到实绩的元素：elm_null[{0}]", elm_null);

							Log::Trace("", __FUNCTION__, "formula_value[{0}],s_elm_code_name[{1}]", formula_value, s_elm_code_name);

							//cmd_inq_01.SetCommandText(" SELECT replace(@formula_value,@elm_name,@elm_value) FROM DUAL ");
							//cmd_inq_01.Parameters.Set("formula_value", formula_value);
							//cmd_inq_01.Parameters.Set("elm_name", s_elm_code_name.ToUpper());
							//cmd_inq_01.Parameters.Set("elm_value", 0); ////找不到实绩，赋值0
							//cmd_inq_01.ExecuteReader();
							//if (cmd_inq_01.Read())
							//{
							//	formula_value = cmd_inq_01.GetString(1);
							//	Log::Trace("", __FUNCTION__, "formula_value[{0}]", formula_value);
							//}
							//cmd_inq_01.Close();
							formula_value = formula_value.Replace(s_elm_code_name, '0');
						}

					}
				}
				cmd_inq_qmys.Close();
				Log::Trace("", __FUNCTION__, "[{0}]实绩公式(代入值后)[{1}]", s_elm_fmla_code_1, formula_value);

				if (elm_null.TrimOrBlank() != " ")
				{
					//if (elm_null.SubstringNE(0, 1) == ",")
					//	elm_null = elm_null.SubstringNE(1, elm_null.GetLength() - 1);
					//CFormattable arguments[] = { elm_null.Trim() };
					//CMessageFormat::Format(s.msg, _RES("QM00S0005939")/*元素[{0}]缺少实绩，请录入这些元素的实绩！*/, arguments, 1);
					//throw CApplicationException(-1, s.msg, log.Location);
				}

				try
				{
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
// DM8 适配 CHANGE-194:查询。SYSIBM 辅助表改为 DUAL。
// 改写原因：SYSIBM 辅助表改为 DUAL；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
						// sqlstr = " SELECT ROUND(" + formula_value + +",4) FROM SYSIBM.DUAL  ";
// DM8 SQL：
						sqlstr = " SELECT ROUND(" + formula_value + +",4) FROM DUAL  ";
						break;
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
						sqlstr = " SELECT ROUND(" + formula_value + +",4) FROM DUAL ";
						break;
					default:						// 所有数据库适用，通用SQL语句
// DM8 适配 CHANGE-195:查询。SYSIBM 辅助表改为 DUAL。
// 改写原因：SYSIBM 辅助表改为 DUAL；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
						// sqlstr = " SELECT ROUND(" + formula_value + +",4) FROM SYSIBM.DUAL ";
// DM8 SQL：
						sqlstr = " SELECT ROUND(" + formula_value + +",4) FROM DUAL ";
						break;
					}
					cmd_inq_03.SetCommandText(sqlstr);
					cmd_inq_03.ExecuteReader();
					if (cmd_inq_03.Read())
					{
						s_elm_value = cmd_inq_03.GetDecimal(1);
					}
					cmd_inq_03.Close();
				}
				catch (const CException& ex)
				{
					cmd_inq_01.Close();
					strcpy(s.msg, _RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", __FUNCTION__, "[{0}]实绩公式计算出的值[{1}]", s_elm_fmla_code_1, s_elm_value);
				
			}

		}
		cmd_inq.Close();

		bcls_ret->Tables[0].Rows[0]["ELM_FMLA_VALUE"] = s_elm_value;
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		Log::Trace("", "", "1111111111111111");
		//CFormattable arguments[] = { ex.GetCode() };
		//CMessageFormat::Format(s.msg,  "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。", arguments, 1);

		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		Log::Trace("", "", "%s", (const char*)ex.GetMsg());
		sprintf(s.msg, "erorr[%s]", (const char*)ex.GetMsg());
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
		Log::Trace("", "", "s.msg = [%s]", s.msg);
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		Log::Trace("", "", "2222222222222222");
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		Log::Trace("", "", "3333333333333333");
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		strncpy(s.sysmsg, "System Exception", sizeof(s.sysmsg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;

}
