#pragma once

/**
 * @file EngineResult.h
 * @brief Standard result object returned by all Velnix engine operations.
 *
 * EngineResult encapsulates the outcome of an operation by combining:
 *  - Success/failure status
 *  - EngineError code
 *  - Optional descriptive message
 *
 * This avoids using bool or throwing exceptions for expected runtime failures.
 */

#include <string>
#include <utility>

#include "../Error/EngineError.h"

namespace velnix::core
{
    class EngineResult
    {
    public:

        /// Creates a successful result.
        static EngineResult Success()
        {
            return EngineResult(EngineError::None, "");
        }

        /// Creates a failure result.
        static EngineResult Failure(
            EngineError error,
            std::string message = "")
        {
            return EngineResult(error, std::move(message));
        }

        /// Returns true if the operation succeeded.
        [[nodiscard]]
        bool IsSuccess() const noexcept
        {
            return m_error == EngineError::None;
        }

        /// Returns true if the operation failed.
        [[nodiscard]]
        bool IsFailure() const noexcept
        {
            return !IsSuccess();
        }

        /// Allows:
        /// if(result)
        /// if(!result)
        explicit operator bool() const noexcept
        {
            return IsSuccess();
        }

        /// Returns the error code.
        [[nodiscard]]
        EngineError Error() const noexcept
        {
            return m_error;
        }

        /// Returns the optional message.
        [[nodiscard]]
        const std::string& Message() const noexcept
        {
            return m_message;
        }

    private:

        EngineResult(
            EngineError error,
            std::string message)
            :
            m_error(error),
            m_message(std::move(message))
        {
        }

        EngineError m_error;

        std::string m_message;
    };
}