/*
编译: csc BinCipherGen.cs
使用: BinCipherGen xxx.bin
注	:  加密-->每256个字节的前4个字节与CIPHERMAGIC异或
	   生成文件--->  xxx.dwm
*/

using System;
using System.IO;
public class BinCipherGen{
	
    const ushort CIPHERMAGIC = 0xAABB;
	const ushort CIPHERLENGTH = 256;
	
	static void Main(String[] args){
		
		if(args.Length <= 0){
			return ;
		}
		
		int rlen = 0;
		Byte [] TmpBuffer  = new Byte[256];
		String FullPathBuffer = Path.GetFullPath(args[0]);/*获取文件全路径*/
		String OutPathBuffer  = Path.ChangeExtension(args[0], "dwm");/*修改文件后缀名*/
		using(BinaryReader fReader = new BinaryReader(File.Open(@FullPathBuffer, FileMode.Open))){
			using(BinaryWriter fWriter = new BinaryWriter(File.Open(@OutPathBuffer, FileMode.Create))){
				while((rlen = fReader.Read(TmpBuffer, 0, CIPHERLENGTH)) > 0){
					Byte [] cipherBuffer = BitConverter.GetBytes(BitConverter.ToUInt32 (TmpBuffer, 0) ^Convert.ToUInt32(CIPHERMAGIC));
					Array.Copy(cipherBuffer, 0, TmpBuffer, 0, 4);
					fWriter.Write(TmpBuffer, 0, rlen);
				}
			}
				
		}
	}
}