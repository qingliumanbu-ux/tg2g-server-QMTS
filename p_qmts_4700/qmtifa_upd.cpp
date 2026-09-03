/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 任龙
日期: 2012-03-16
功能: 氧化铁粉实绩信息修改
修改历史：
	日期:________；修改人：________; 需求提出人________
	变更内容:

**************************************************/
/*<remark>=========================================================
/// <summary>
/// 氧化铁粉实绩信息修改
/// <para>
/// 1. 读取前台传入参数；
/// 2. 获取表列名；
/// 3. 建立修改语句；
/// 4. 执行修改操作，返回修改结果。
/// </para>
/// </summary>
/// <param name="TABLE_NAME">界面名称</param>
/// <param name="sql_upd">拼接sql语句</param>
/// <returns>修改基表信息</returns>
===========================================================</remark>*/


#include "stdafx.h"


BM2F_ENTERACE(qmtifa_upd);


int f_qmtifa_upd(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	/*系统日志类定义*/
	CTracer log(__FUNCTION__);	

	int  doFlag = 0;
	
	/*数据库SQL操作字符串，用于捕获数据库操作异常情况*/
	CString sqlstr = "";	

	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	int logFlag = 1;
	try
	{
		/*获取传入表行数*/
		int count = bcls_rec->Tables[0].Rows.get_Count();
		
		for (int i = 0; i < count ; i++ )
		{				
			/*申明更新sql语句*/
			CString sql_upd = "UPDATE ";
			
			sql_upd += " TQMTIFA";
			
			sql_upd += " SET ";
			
			//在线操作
			CDbCommand cmd(conn);
				
			for(int j = 0;j < bcls_rec->Tables[0].Columns.get_Count();j++)
			{

				/*数据修改sql拼接*/
				if(bcls_rec->Tables[0].Columns[j].get_ColumnName() == "REC_REVISOR")
				{
					sql_upd += bcls_rec->Tables[0].Columns[j].get_ColumnName() +" = @rec_revisor,";
					cmd.Parameters.Set("rec_revisor",s.userid);
				}
				else if(bcls_rec->Tables[0].Columns[j].get_ColumnName() == "REC_REVISE_TIME")
				{
					sql_upd += bcls_rec->Tables[0].Columns[j].get_ColumnName() +" = @rec_revise_time,";
					cmd.Parameters.Set("rec_revise_time",datetimeNow);
				}
				else if(bcls_rec->Tables[0].Columns[j].get_ColumnName() != "LOT_NO_PE")
				{
					sql_upd += bcls_rec->Tables[0].Columns[j].get_ColumnName()+" = @" + bcls_rec->Tables[0].Columns[j].get_ColumnName()+",";
					cmd.Parameters.Set(bcls_rec->Tables[0].Columns[j].get_ColumnName(),(CString)bcls_rec->Tables[0].Rows[i][j]);
				}
				if(j==bcls_rec->Tables[0].Columns.get_Count()-1)
				{
					/*截取字符串的最后一位的逗号*/
					sql_upd=sql_upd.Substring(0,sql_upd.GetLength()-1);
							
					/*打印更新的sql语句*/
					Log::Trace("QMTI", "f_qmtifa_upd","sql_upd_1[%s]",(const char*)sql_upd);
					
					/*sql更新的条件*/
					sql_upd += " WHERE LOT_NO_PE =@lot_no_pe ";
					/*赋值LOT_NO_PE*/
					cmd.Parameters.Set("lot_no_pe",(CString)bcls_rec->Tables[0].Rows[i]["LOT_NO_PE"].ToString().Trim());
				}
			}
			
			Log::Trace("QMTI", "f_qmtifa_upd","sql_t[%s]",(const char*)sql_upd);
			
			//连接数据库
			sqlstr=sql_upd;
			cmd.SetCommandText(sql_upd);
			cmd.ExecuteNonQuery();
		}
	}

	/*捕获数据库操作异常*/
	catch(CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = ex.GetMsg() + "\r\n" + sqlstr;
		/*返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应*/
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);	
		s.flag = -1;
		/*数据库异常时返回-1，事务将被回滚*/
		doFlag = -1;   
	}
	/*捕获应用错误*/
	catch(CApplicationException& ex)
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

	s.flag = doFlag;

	return(doFlag);
}
