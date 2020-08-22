//From Luna Tsukinashi
#pragma once
#include "mbed.h"
#include "string.h"

////////////////////////////////////////////////////////////////////////////////
/// @file           circularBuffer.cpp
/// @brief          mbed向けリングバッファライブラリライブラリ
/// @author         LunaTsukinashi
/// @date           2020-07-29
/// @version        1.00
/// @note           ファイルに備考などを明記する場合はここへ書き込む
/// @attention      修正済み
/// @par            History
///                 Ver1.00 初版
///
/// Copyright (c) Luna Tsukinashi All Rights reserved.
///
/// - 本ソフトウェアの一部又は全てを無断で複写複製（コピー）することは、
///   著作権侵害にあたりますので、これを禁止します。
/// - 本製品の使用に起因する侵害または特許権その他権利の侵害に関しては
///   私は一切その責任を負いません。
///
////////////////////////////////////////////////////////////////////////////////

namespace lunaLib{
	template<typename T, uint16_t Capacity>
	class CircularBuffer{
	private:
		T buffer[Capacity];
		const uint16_t m_mask;
		uint16_t m_head, m_tail;
		uint16_t m_size;
		uint16_t m_capacity;

		inline void MovePointer(uint16_t &pointer, const int16_t &step){
			pointer = (pointer + step) & m_mask;
		}
		inline uint16_t emptySize() const{
			return Capacity - m_size;
		}

	public:
		volatile bool isLocked;
		explicit CircularBuffer() :m_mask(Capacity - 1), m_head(0), m_tail(0), m_size(0), m_capacity(Capacity), isLocked(false){
			if ((Capacity & (Capacity - 1)) != 0) while (true);
		}

		virtual ~CircularBuffer(){
		}

		void push_back(const T &value){
			buffer[m_tail] = value;
			MovePointer(m_tail, 1);
			if (m_size == Capacity)
				MovePointer(m_head, 1);
			else m_size++;
		}
		T pull(){
			const T res = buffer[m_head];
			MovePointer(m_head, 1);
			m_size--;
			return res;
		}

		uint16_t size() const{
			return m_size;
		}

		void clear(){
			m_head = 0;
			m_tail = 0;
			m_size = 0;
		}

		T &operator[ ](const uint16_t &n){
			return buffer[(m_head + n) & m_mask];
		}
	};

};
