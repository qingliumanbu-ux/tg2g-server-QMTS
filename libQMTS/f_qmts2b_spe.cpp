/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-18
Description: 板坯成分组合元素计算
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中
#include "tqmts2c.h"
#include "tqmts0x.h"

/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///板坯成分组合元素计算
/// <para>
/// 1.板坯成分组合元素计算。
/// 
/// </para>
/// <para>数据库表：TQMTS2C(板坯成分实绩表)     </para>
/// <para>主调用函数：qmts2b_ins(),qmts2b_upd()	</para>
/// <para>需调用函数：							</para>
/// </summary>
/// <param name="ST_SAMPLE_NO">试样号			</param>
/// <param name="ST_NO">出钢记号				</param>
/// <returns>  </returns>
===========================================================</remark>*/


int f_qmts2b_spe(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString formula_value = "";
	CString elm_null = "";
	int formula_len = 0;

	CString sqlstr = "";

	/* 实体类定义 */
	CTQMTS2C tqmts2c(conn);
	CTQMTS0X tqmts0x(conn);

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_01(conn);
	CDbCommand cmd_inq_02(conn);
	CDbCommand cmd_del(conn);

	//初始化
	s.flag = 0;
	s.sqlcode = 0;
	strcpy(s.msg,  " ");
	//20130427 HYF SubstringNE
	try
	{
		/*获得传入参数*/
		tqmts2c.ST_SAMPLE_NO = bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString().Trim();
		tqmts2c.ST_NO = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();

		Log::Trace("", __FUNCTION__, "f_qmts2b_spe IN:---st_sample_no[{0}]",(const char*)tqmts2c.ST_SAMPLE_NO);
		Log::Trace("", __FUNCTION__, "f_qmts2b_spe IN:---st_no[{0}]",(const char*)tqmts2c.ST_NO);

		//校验传入参数
		if(tqmts2c.ST_SAMPLE_NO.TrimOrBlank() == " ")
		{
			Log::Trace("", __FUNCTION__, "ERROR------[试样号不允许为空]");
			strcpy(s.msg,_RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if(tqmts2c.ST_NO.TrimOrBlank() == " ")
		{
			Log::Trace("", __FUNCTION__, "ERROR------[出钢记号不允许为空]");
			strcpy(s.msg,_RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		/**********目前组合元素只有CEQ\PCM公式分别对应tqmts0x.ELM_FMLA_CODE1\tqmts0x.ELM_FMLA_CODE2**********/
		//删除已有组合元素实绩值，重新计算
		cmd_del.SetCommandText(" DELETE TQMTS2C WHERE ST_SAMPLE_NO = @st_sample_no AND ELM_CODE IN ('C01','C03') ");
		cmd_del.Parameters.Set("st_sample_no", tqmts2c.ST_SAMPLE_NO);
		cmd_del.ExecuteNonQuery();
		cmd_del.Close();
		//获得组合元素公式代码
		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT ELM_FMLA_CODE1,ELM_FMLA_CODE2 "
				"  FROM TQMTS0X "
				" WHERE ST_NO = @st_no ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("st_no", tqmts2c.ST_NO.Trim());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tqmts0x.ELM_FMLA_CODE1 = cmd_inq.GetString(1);
			tqmts0x.ELM_FMLA_CODE2 = cmd_inq.GetString(2);
		}
		cmd_inq.Close();
		Log::Trace("", __FUNCTION__, "【CEQ】公式代码[{0}]，【PCM】公式代码[{1}]",(const char*)tqmts0x.ELM_FMLA_CODE1,(const char*)tqmts0x.ELM_FMLA_CODE2);
		if (tqmts0x.ELM_FMLA_CODE1.TrimOrBlank() != " " && tqmts0x.ELM_FMLA_CODE1.TrimOrBlank() != "00")
		{
			//获得组合元素【CEQ】公式代码
			switch(conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT CODE_DESC_2_CONTENT "
					"  FROM TEP0002 "
					" WHERE CODE_CLASS = 'QM63' "
					"   AND CODE = @code ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("code", tqmts0x.ELM_FMLA_CODE1.Trim());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				formula_value = cmd_inq.GetString(1);
				Log::Trace("", __FUNCTION__, "【CEQ】公式[{0}]",(const char*)formula_value);

				switch(conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT * "
						"  FROM TQMTS2C "
						" WHERE ST_SAMPLE_NO = @st_sample_no"
						" ORDER BY lengthb(ELM_NAME) desc ";
					break;
				}
				cmd_inq_01.SetCommandText(sqlstr);
				cmd_inq_01.Parameters.Set("st_sample_no", tqmts2c.ST_SAMPLE_NO);
				cmd_inq_01.ExecuteReader();
				while (cmd_inq_01.Read())
				{
					cmd_inq_01.Fetch(tqmts2c);
					cmd_inq_02.SetCommandText(" SELECT replace(@formula_value,@elm_name,@elm_value) FROM DUAL ");
					cmd_inq_02.Parameters.Set("formula_value", formula_value);
					cmd_inq_02.Parameters.Set("elm_name", tqmts2c.ELM_NAME);
					cmd_inq_02.Parameters.Set("elm_value", tqmts2c.ELM_VALUE);
					cmd_inq_02.ExecuteReader();
					if(cmd_inq_02.Read())
					{
						formula_value = cmd_inq_02.GetString(1);
					}
					cmd_inq_02.Close();
				}
				cmd_inq_01.Close();
				Log::Trace("", __FUNCTION__, "【CEQ】公式(代入值后)[{0}]",(const char*)formula_value);

				elm_null = formula_value;
				formula_len = formula_value.GetLength();
				Log::Trace("", __FUNCTION__, "formula_len[{0}]",formula_len);
				for(i=0; i<formula_len;)
				{
					if((elm_null.SubstringNE(i,1)<="9" && elm_null.SubstringNE(i,1)>="0")
						||elm_null.SubstringNE(i,1)=="."
						||elm_null.SubstringNE(i,1)=="("
						||elm_null.SubstringNE(i,1)==")")
					{
						if(i>0)
						{
							elm_null = elm_null.SubstringNE(0,i) + elm_null.SubstringNE(i+1,formula_len);
						}
						else
						{
							elm_null = elm_null.SubstringNE(i+1,formula_len);
						}
						formula_len--;
						Log::Trace("", __FUNCTION__, "0~9 elm_null[{0}]formula_len[{1}]i[{2}]",(const char*)elm_null,formula_len,i);
					}
					else if(elm_null.SubstringNE(i,1)=="+"
						||elm_null.SubstringNE(i,1)=="-"
						||elm_null.SubstringNE(i,1)=="*"
						||elm_null.SubstringNE(i,1)=="/")
					{
						if(i>0)
						{
							if(elm_null.SubstringNE(i-1,1)==",")
							{
								elm_null = elm_null.SubstringNE(0,i) + elm_null.SubstringNE(i+1,formula_len);
								formula_len--;
								Log::Trace("", __FUNCTION__, ", elm_null[{0}]formula_len[{1}]i[{2}]",(const char*)elm_null,formula_len,i);
							}
							else
							{
								elm_null[i] = ',';
								i++;
								Log::Trace("", __FUNCTION__, "!, elm_null[{0}]formula_len[{1}]i[{2}]",(const char*)elm_null,formula_len,i);
							}
						}
						else
						{
							elm_null = elm_null.SubstringNE(i+1,formula_len);
							formula_len--;
							Log::Trace("", __FUNCTION__, "i0 elm_null[{0}]formula_len[{1}]i[{2}]",(const char*)elm_null,formula_len,i);
						}
					}
					else
					{
						i++;
						Log::Trace("", __FUNCTION__, "elm elm_null[{0}]formula_len[{1}]i[{2}]",(const char*)elm_null,formula_len,i);
					}
				}
				Log::Trace("", __FUNCTION__, "elm_null = [{0}]",(const char*)elm_null);
				if(elm_null.TrimOrBlank() != " ")
				{
					if(elm_null.SubstringNE(elm_null.GetLength()-1,1) == ",")
						elm_null = elm_null.SubstringNE(0,elm_null.GetLength()-1);
					CFormattable arguments[] = { elm_null.Trim() };
					CMessageFormat::Format(s.msg,  _RES("QM00S0005939")/*元素[{0}]缺少实绩，请录入这些元素的实绩！*/, arguments, 1);
					throw CApplicationException(-1, s.msg, log.Location);
				}

				try
				{
					cmd_inq_01.SetCommandText(" SELECT ROUND(" + formula_value + + ",2) FROM DUAL ");
					cmd_inq_01.ExecuteReader();
					if (cmd_inq_01.Read())
					{
						tqmts2c.ELM_VALUE = cmd_inq_01.GetDecimal(1);
					}
					cmd_inq_01.Close();
				}
				catch(const CException& ex)
				{
					cmd_inq_01.Close();
					strcpy(s.msg, _RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", __FUNCTION__, "【CEQ】公式计算出的值[{0}]",tqmts2c.ELM_VALUE.ToDouble());

				tqmts2c.ELM_CODE = "C01";
				tqmts2c.ELM_NAME = "CEQ";
				tqmts2c.ELM_OK = 0;

				tqmts2c.Insert();
			}
			else
			{
				Log::Trace("", __FUNCTION__, "ERROR------[组合元素【CEQ】公式代码内容未配置]");
				strcpy(s.msg,_RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			cmd_inq.Close();
		}
		if (tqmts0x.ELM_FMLA_CODE2.TrimOrBlank() != " " && tqmts0x.ELM_FMLA_CODE2.TrimOrBlank() != "00")
		{
			//获得组合元素【PCM】公式代码
			switch(conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT CODE_DESC_2_CONTENT "
					"  FROM TEP0002 "
					" WHERE CODE_CLASS = 'QMA1' "
					"   AND CODE = @code ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("code", tqmts0x.ELM_FMLA_CODE2.Trim());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				formula_value = cmd_inq.GetString(1);
				Log::Trace("", __FUNCTION__, "【PCM】公式[{0}]",(const char*)formula_value);

				switch(conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT * "
						"  FROM TQMTS2C "
						" WHERE ST_SAMPLE_NO = @st_sample_no"
						" ORDER BY lengthb(ELM_NAME) desc ";
					break;
				}
				cmd_inq_01.SetCommandText(sqlstr);
				cmd_inq_01.Parameters.Set("st_sample_no", tqmts2c.ST_SAMPLE_NO);
				cmd_inq_01.ExecuteReader();
				while (cmd_inq_01.Read())
				{
					cmd_inq_01.Fetch(tqmts2c);
					cmd_inq_02.SetCommandText(" SELECT replace(@formula_value,@elm_name,@elm_value) FROM DUAL ");
					cmd_inq_02.Parameters.Set("formula_value", formula_value);
					cmd_inq_02.Parameters.Set("elm_name", tqmts2c.ELM_NAME);
					cmd_inq_02.Parameters.Set("elm_value", tqmts2c.ELM_VALUE);
					cmd_inq_02.ExecuteReader();
					if(cmd_inq_02.Read())
					{
						formula_value = cmd_inq_02.GetString(1);
					}
					cmd_inq_02.Close();
				}
				cmd_inq_01.Close();
				Log::Trace("", __FUNCTION__, "【PCM】公式(代入值后)[{0}]",(const char*)formula_value);

				elm_null = formula_value;
				formula_len = formula_value.GetLength();
				Log::Trace("", __FUNCTION__, "formula_len[{0}]",formula_len);
				for(i=0; i<formula_len;)
				{
					if((elm_null.SubstringNE(i,1)<="9" && elm_null.SubstringNE(i,1)>="0")
						||elm_null.SubstringNE(i,1)=="."
						||elm_null.SubstringNE(i,1)=="("
						||elm_null.SubstringNE(i,1)==")")
					{
						if(i>0)
						{
							elm_null = elm_null.SubstringNE(0,i) + elm_null.SubstringNE(i+1,formula_len);
						}
						else
						{
							elm_null = elm_null.SubstringNE(i+1,formula_len);
						}
						formula_len--;
						Log::Trace("", __FUNCTION__, "0~9 elm_null[{0}]formula_len[{1}]i[{2}]",(const char*)elm_null,formula_len,i);
					}
					else if(elm_null.SubstringNE(i,1)=="+"
						||elm_null.SubstringNE(i,1)=="-"
						||elm_null.SubstringNE(i,1)=="*"
						||elm_null.SubstringNE(i,1)=="/")
					{
						if(i>0)
						{
							if(elm_null.SubstringNE(i-1,1)==",")
							{
								elm_null = elm_null.SubstringNE(0,i) + elm_null.SubstringNE(i+1,formula_len);
								formula_len--;
								Log::Trace("", __FUNCTION__, ", elm_null[{0}]formula_len[{1}]i[{2}]",(const char*)elm_null,formula_len,i);
							}
							else
							{
								elm_null[i] = ',';
								i++;
								Log::Trace("", __FUNCTION__, "!, elm_null[{0}]formula_len[{1}]i[{2}]",(const char*)elm_null,formula_len,i);
							}
						}
						else
						{
							elm_null = elm_null.SubstringNE(i+1,formula_len);
							formula_len--;
							Log::Trace("", __FUNCTION__, "i0 elm_null[{0}]formula_len[{1}]i[{2}]",(const char*)elm_null,formula_len,i);
						}
					}
					else
					{
						i++;
						Log::Trace("", __FUNCTION__, "elm elm_null[{0}]formula_len[{1}]i[{2}]",(const char*)elm_null,formula_len,i);
					}
				}
				Log::Trace("", __FUNCTION__, "elm_null = [{0}]",(const char*)elm_null);
				if(elm_null.TrimOrBlank() != " ")
				{
					if(elm_null.SubstringNE(elm_null.GetLength()-1,1) == ",")
						elm_null = elm_null.SubstringNE(0,elm_null.GetLength()-1);
					CFormattable arguments[] = { elm_null.Trim() };
					CMessageFormat::Format(s.msg,  _RES("QM00S0005939")/*元素[{0}]缺少实绩，请录入这些元素的实绩！*/, arguments, 1);
					throw CApplicationException(-1, s.msg, log.Location);
				}

				try
				{
					cmd_inq_01.SetCommandText(" SELECT ROUND(" + formula_value + + ",2) FROM DUAL ");
					cmd_inq_01.ExecuteReader();
					if (cmd_inq_01.Read())
					{
						tqmts2c.ELM_VALUE = cmd_inq_01.GetDecimal(1);
					}
					cmd_inq_01.Close();
				}
				catch(const CException& ex)
				{
					cmd_inq_01.Close();
					strcpy(s.msg, _RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", __FUNCTION__, "【PCM】公式计算出的值[{0}]",tqmts2c.ELM_VALUE.ToDouble());

				tqmts2c.ELM_CODE = "C03";
				tqmts2c.ELM_NAME = "PCM";
				tqmts2c.ELM_OK = 0;

				tqmts2c.Insert();
			}
			else
			{
				Log::Trace("", __FUNCTION__, "ERROR------[组合元素【PCM】公式代码内容未配置]");
				strcpy(s.msg,_RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			cmd_inq.Close();
		}
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}

