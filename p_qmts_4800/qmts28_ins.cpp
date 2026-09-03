/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-20
Description: 新增宏观检化验委托信息
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///新增宏观检化验委托信息
/// <para>
/// 1.新增宏观检化验委托信息
///
/// </para>
/// <para>数据库表：TQMTS28(宏观检化验委托信息表)		   </para>
/// <para>主调用函数：前台QMTS28画面的F3(新增)调用		   </para>
/// <para>需调用函数：							           </para>
/// </summary>
/// <param name="SLAB_NO">  材料号				           </param>
/// <returns>  </returns>
===========================================================</remark>*/

// service入口
BM2F_ENTERACE(qmts28_ins)


int f_qmts28_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;
	CString s_new_heat_no = " ";
	CString s_new_pono = " ";
	CString s_max_char = " ";
	CDecimal v_count = 0;
	CString s_sample_pos_code = " ";
	CString sqlstr = "";

	/* 实体类定义 */
	CModel tqmts2a("TQMTS2A");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		//新增宏观检化验委托信息
		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//取得单行传入信息
			tqmts2a.Reset();
			tqmts2a.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
			{
				tqmts2a["SLAB_NO"] = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString().Trim();
			}
			Log::Trace("", "", "qmts28_ins IN:---SLAB_NO = [{0}]", (const char*)tqmts2a["SLAB_NO"].ToString());
			Log::Trace("", "", "qmts28_ins IN:---SAMPLE_POS_CODE = [{0}]", (const char*)tqmts2a["SAMPLE_POS_CODE"].ToString());
			Log::Trace("", "", "qmts28_ins IN:---HEAT_NO = [{0}]", (const char*)tqmts2a["HEAT_NO"].ToString());

			//校验传入参数
			if (tqmts2a["SLAB_NO"].ToString().TrimOrBlank() == " ")
			{
				strcpy(s.msg, _RES("GCRSS0000035")/*材料号不能为空*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//if (tqmts2a["SAMPLE_POS_CODE"].ToString().TrimOrBlank() == " ")
			//{
			//	strcpy(s.msg, _RES("QM00S0004341")/*取样位置不可为空。*/);
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}
			tqmts2a["SAMPLE_POS_CODE"] = "A";
			s_sample_pos_code = " ";
			if (tqmts2a["SAMPLE_POS_CODE"].ToString().TrimOrBlank() == "D")//头+尾生成两笔委托
			{
				s_sample_pos_code = tqmts2a["SAMPLE_POS_CODE"];
				tqmts2a["SAMPLE_POS_CODE"] = "A";
			}
			v_count = 0;
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT count(1) "
					"  FROM TQMTS2A "
					" WHERE SLAB_NO = @mat_no "
					"   AND SAMPLE_POS_CODE = @sample_pos_code";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("mat_no", tqmts2a["SLAB_NO"].ToString().Trim());
			cmd_inq.Parameters.Set("sample_pos_code", tqmts2a["SAMPLE_POS_CODE"].ToString().Trim());
			v_count = cmd_inq.ExecuteScalar();
			cmd_inq.Close();
			if (v_count > 0)
			{
				sprintf(s.msg, "材料号[%s]取样位置[%s]的宏观检化验委托已存在，不需要再生成委托", (const char*)tqmts2a["SLAB_NO"].ToString(), (const char*)tqmts2a["SAMPLE_POS_CODE"].ToString());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (i == 0)  //////多个材料一起委托，变同一个炉号 update by yiling 20160908
			{
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT HEAT_NO,PONO,ST_NO "
						"  FROM TMMSM01 "
						" WHERE MAT_NO = @mat_no ";
					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("mat_no", tqmts2a["SLAB_NO"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tqmts2a["HEAT_NO"] = cmd_inq.GetString(1);
					tqmts2a["PONO"] = cmd_inq.GetString(2);
					tqmts2a["ST_NO"] = cmd_inq.GetString(3);
				}
				cmd_inq.Close();
				Log::Trace("", "", "tqmts2a.HEAT_NO         	  = [{0}]", (const char*)tqmts2a["HEAT_NO"].ToString());
				Log::Trace("", "", "tqmts2a.ST_NO           	  = [{0}]", (const char*)tqmts2a["ST_NO"].ToString());
				Log::Trace("", "", "tqmts2a.PONO            	  = [{0}]", (const char*)tqmts2a["PONO"].ToString());
				if (tqmts2a["ST_NO"].ToString().TrimOrBlank() == " ")
				{
					strcpy(s.msg, "内部钢种不可为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				////生成新炉号，update by yiling 20160713  
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT substr(MAX(HEAT_NO),length(MAX(HEAT_NO)),1)"
						"  FROM TQMTS2A "
						" WHERE HEAT_NO like @heat_no||'%' ";;
					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", tqmts2a["HEAT_NO"].ToString().Trim());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					s_max_char = cmd_inq.GetString(1);
					Log::Trace("", "", "s_max_char= [{0}]", s_max_char);
					if (s_max_char.TrimOrBlank() != " ")
					{
						if (s_max_char >= 65 && s_max_char < 90)
						{
							s_max_char = s_max_char[0] + 1;
						}
						else
						{
							s_max_char = "A";
						}
						Log::Trace("", "", "2233s_max_char= [{0}]", s_max_char);
					}
					else
					{
						s_max_char = "A";
					}
				}
				else
				{
					s_max_char = "A";
				}
				cmd_inq.Close();
				Log::Trace("", "", "s_max_char = [{0}]tqmts2a.HEAT_NO[{1}]", s_max_char, tqmts2a["HEAT_NO"].ToString());
				if (s_max_char.Trim() != "")
				{
					s_new_heat_no = tqmts2a["HEAT_NO"].ToString() + s_max_char;
					s_new_pono = tqmts2a["PONO"].ToString() + s_max_char;
				}
				else
				{
					strcpy(s.msg, "熔炼号自动转换失败！");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}

			Log::Trace("", "", "s_new_heat_no = [{0}]tqmts2a.HEAT_NO[{1}]", s_new_heat_no, tqmts2a["HEAT_NO"].ToString());
			Log::Trace("", "", "s_new_pono = [{0}]tqmts2a.PONO[{1}]", s_new_pono, tqmts2a["PONO"].ToString());
			tqmts2a["HEAT_NO"] = s_new_heat_no;
			tqmts2a["PONO"] = s_new_pono;
			/******************** 赋初值 *************************/
			tqmts2a["REC_CREATOR"] = s.userid;
			tqmts2a["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts2a["REC_REVISOR"] = " ";
			tqmts2a["REC_REVISE_TIME"] = " ";
			tqmts2a["ARCHIVE_FLAG"] = " ";
			tqmts2a["DU_FLAG"] = " ";
			tqmts2a["DU_MAKER"] = " ";
			tqmts2a["DU_TIME"] = " ";
			tqmts2a["VERSION"] = 1;

			tqmts2a["NEED_C"] = "1";
			tqmts2a["NEED_SI"] = "1";
			tqmts2a["NEED_MN"] = "1";
			tqmts2a["NEED_P"] = "1";
			tqmts2a["NEED_S"] = "1";
			tqmts2a["NEED_CR"] = "1";
			tqmts2a["NEED_NI"] = "1";
			tqmts2a["NEED_CU"] = "1";
			tqmts2a["NEED_MO"] = "1";
			tqmts2a["NEED_W"] = "1";
			tqmts2a["NEED_V"] = "1";
			tqmts2a["NEED_TI"] = "1";
			tqmts2a["NEED_AL"] = "1";
			tqmts2a["NEED_PB"] = "1";
			tqmts2a["NEED_SN"] = "1";
			tqmts2a["NEED_SB"] = "1";
			tqmts2a["NEED_AS"] = "1";
			tqmts2a["NEED_CA"] = "1";
			tqmts2a["NEED_B"] = "1";
			tqmts2a["NEED_N"] = "1";
			tqmts2a["NEED_H"] = "1";
			tqmts2a["NEED_O"] = "1";
			tqmts2a["NEED_BI"] = "1";
			tqmts2a["NEED_RE"] = "0";
			tqmts2a["NEED_ZN"] = "0";
			tqmts2a["NEED_ZR"] = "0";
			tqmts2a["NEED_BS"] = "0";
			tqmts2a["NEED_TA"] = "0";
			tqmts2a["NEED_FE"] = "0";
			tqmts2a["NEED_CE"] = "0";
			tqmts2a["NEED_AG"] = "0";
			tqmts2a["NEED_LA"] = "0";
			tqmts2a["NEED_AL_S"] = "0";
			tqmts2a["NEED_ALOXY"] = "0";
			tqmts2a["NEED_B_S"] = "0";
			tqmts2a["NEED_BOXY"] = "0";
			tqmts2a["NEED_AL_T"] = "0";
			tqmts2a["NEED_SE"] = "0";
			tqmts2a["NEED_MG"] = "0";
			tqmts2a["NEED_CD"] = "0";
			tqmts2a["NEED_CEQ"] = "0";
			tqmts2a["NEED_MNEQ"] = "0";
			tqmts2a["NEED_PCM"] = "0";
			tqmts2a["NEED_DI"] = "0";
			tqmts2a["NEED_SP1"] = "0";
			tqmts2a["NEED_SP2"] = "0";
			tqmts2a["NEED_SP3"] = "0";
			tqmts2a["NEED_ISE"] = "0";
			tqmts2a["NEED_ISE1"] = "0";

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT CODE_DESC_1_CONTENT "
					"  FROM TEP0002 "
					" WHERE CODE_CLASS = 'QM45' "
					"   AND CODE = @sample_pos_code ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("sample_pos_code", tqmts2a["SAMPLE_POS_CODE"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tqmts2a["SAMPLE_POS"] = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
			Log::Trace("", "", "tqmts2a.SAMPLE_POS         	  = [{0}]", (const char*)tqmts2a["SAMPLE_POS"].ToString());

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT CODE_DESC_1_CONTENT "
					"  FROM TEP0002 "
					" WHERE CODE_CLASS = 'QMZE' "
					"   AND CODE = @sample_mode_code ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("sample_mode_code", tqmts2a["SAMPLE_MODE_CODE"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tqmts2a["SAMPLE_MODE"] = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
			Log::Trace("", "", "tqmts2a.SAMPLE_MODE         	  = [{0}]", (const char*)tqmts2a["SAMPLE_MODE"].ToString());

			tqmts2a["SEND_NUM"] = 0;

			tqmts2a.TrimOrBlank();
			tqmts2a.Insert();

			if (s_sample_pos_code.TrimOrBlank() == "D")//头+尾生成两笔委托
			{
				tqmts2a["SAMPLE_POS_CODE"] = "C";

				v_count = 0;
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT count(1) "
						"  FROM TQMTS2A "
						" WHERE SLAB_NO = @mat_no "
						"   AND SAMPLE_POS_CODE = @sample_pos_code";
					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("mat_no", tqmts2a["SLAB_NO"].ToString().Trim());
				cmd_inq.Parameters.Set("sample_pos_code", tqmts2a["SAMPLE_POS_CODE"].ToString().Trim());
				v_count = cmd_inq.ExecuteScalar();
				cmd_inq.Close();
				if (v_count > 0)
				{
					sprintf(s.msg, "材料号[%s]取样位置[%s]的宏观检化验委托已存在，不需要再生成委托", (const char*)tqmts2a["SLAB_NO"].ToString(), (const char*)tqmts2a["SAMPLE_POS_CODE"].ToString());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (i == 0)  //////多个材料一起委托，变同一个炉号 update by yiling 20160908
				{
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = "SELECT HEAT_NO,PONO,ST_NO "
							"  FROM TMMSM01 "
							" WHERE MAT_NO = @mat_no ";
						break;
					}
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("mat_no", tqmts2a["SLAB_NO"].ToString());
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						tqmts2a["HEAT_NO"] = cmd_inq.GetString(1);
						tqmts2a["PONO"] = cmd_inq.GetString(2);
						tqmts2a["ST_NO"] = cmd_inq.GetString(3);
					}
					cmd_inq.Close();
					Log::Trace("", "", "tqmts2a.HEAT_NO         	  = [{0}]", (const char*)tqmts2a["HEAT_NO"].ToString());
					Log::Trace("", "", "tqmts2a.ST_NO           	  = [{0}]", (const char*)tqmts2a["ST_NO"].ToString());
					Log::Trace("", "", "tqmts2a.PONO            	  = [{0}]", (const char*)tqmts2a["PONO"].ToString());
					////生成新炉号，update by yiling 20160713  
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = "SELECT  substr(MAX(HEAT_NO),length(MAX(HEAT_NO)),1)"
							"  FROM TQMTS2A "
							" WHERE HEAT_NO like @heat_no||'%' ";
						break;
					}
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("heat_no", tqmts2a["HEAT_NO"].ToString().Trim());
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						s_max_char = cmd_inq.GetString(1);
						Log::Trace("", "", "s_max_char= [{0}]", s_max_char);
						if (s_max_char.TrimOrBlank() != " ")
						{

							if (s_max_char >= 65 && s_max_char < 90)
							{
								s_max_char = s_max_char[0] + 1;
							}
							else
							{
								s_max_char = "A";
							}
							Log::Trace("", "", "222s_max_char= [{0}]", s_max_char);
						}
						else
						{
							s_max_char = "A";
						}
					}
					else
					{
						s_max_char = "A";
					}
					cmd_inq.Close();
					Log::Trace("", "", "s_max_char = [{0}]tqmts2a.HEAT_NO[{1}]", s_max_char, tqmts2a["HEAT_NO"].ToString());
					if (s_max_char.Trim() != "")
					{
						s_new_heat_no = tqmts2a["HEAT_NO"].ToString() + s_max_char;
						s_new_pono = tqmts2a["PONO"].ToString() + s_max_char;
					}
					else
					{
						strcpy(s.msg, "熔炼号自动转换失败！");
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}

				Log::Trace("", "", "s_new_heat_no = [{0}]tqmts2a.HEAT_NO[{1}]", s_new_heat_no, tqmts2a["HEAT_NO"].ToString());
				Log::Trace("", "", "s_new_pono = [{0}]tqmts2a.PONO[{1}]", s_new_pono, tqmts2a["PONO"].ToString());
				tqmts2a["HEAT_NO"] = s_new_heat_no;
				tqmts2a["PONO"] = s_new_pono;
				/******************** 赋初值 *************************/
				tqmts2a["REC_CREATOR"] = s.userid;
				tqmts2a["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tqmts2a["REC_REVISOR"] = " ";
				tqmts2a["REC_REVISE_TIME"] = " ";
				tqmts2a["ARCHIVE_FLAG"] = " ";
				tqmts2a["DU_FLAG"] = " ";
				tqmts2a["DU_MAKER"] = " ";
				tqmts2a["DU_TIME"] = " ";
				tqmts2a["VERSION"] = 1;



				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT CODE_DESC_1_CONTENT "
						"  FROM TEP0002 "
						" WHERE CODE_CLASS = 'QM45' "
						"   AND CODE = @sample_pos_code ";
					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("sample_pos_code", tqmts2a["SAMPLE_POS_CODE"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tqmts2a["SAMPLE_POS"] = cmd_inq.GetString(1);
				}
				cmd_inq.Close();
				Log::Trace("", "", "tqmts2a.SAMPLE_POS         	  = [{0}]", (const char*)tqmts2a["SAMPLE_POS"].ToString());

				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT CODE_DESC_1_CONTENT "
						"  FROM TEP0002 "
						" WHERE CODE_CLASS = 'QMZE' "
						"   AND CODE = @sample_mode_code ";
					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("sample_mode_code", tqmts2a["SAMPLE_MODE_CODE"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tqmts2a["SAMPLE_MODE"] = cmd_inq.GetString(1);
				}
				cmd_inq.Close();
				Log::Trace("", "", "tqmts2a.SAMPLE_MODE         	  = [{0}]", (const char*)tqmts2a["SAMPLE_MODE"].ToString());

				tqmts2a["SEND_NUM"] = 0;

				tqmts2a.TrimOrBlank();
				tqmts2a.Insert();
			}//头+尾生成两笔委托
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
